#include "parser.h"
#include "package.h"
#include "communication.h"

t_identificador mapear_identificador(char *identificador){

    t_identificador id;

    if (string_equals_ignore_case(identificador, "F_READ")){
        id = F_READ;
    }
    else if (string_equals_ignore_case(identificador, "F_WRITE")){
        id = F_WRITE;
    }
    else if (string_equals_ignore_case(identificador, "SET")){
        id = SET;
    }
    else if (string_equals_ignore_case(identificador, "MOV_IN")){
        id = MOV_IN;
    }
    else if (string_equals_ignore_case(identificador, "MOV_OUT")){
        id = MOV_OUT;
    }
    else if (string_equals_ignore_case(identificador, "F_TRUNCATE")){
        id = F_TRUNCATE;
    }
    else if (string_equals_ignore_case(identificador, "F_SEEK")){
        id = F_SEEK;
    }
    else if (string_equals_ignore_case(identificador, "CREATE_SEGMENT")){
        id = CREATE_SEGMENT;
    }
    else if (string_equals_ignore_case(identificador, "I_O")){
        id = I_O;
    }
    else if (string_equals_ignore_case(identificador, "WAIT")){
        id = WAIT;
    }
    else if (string_equals_ignore_case(identificador, "SIGNAL")){
        id = SIGNAL;
    }
    else if (string_equals_ignore_case(identificador, "F_OPEN")){
        id = F_OPEN;
    }
    else if (string_equals_ignore_case(identificador, "F_CLOSE")){
        id = F_CLOSE;
    }
    else if (string_equals_ignore_case(identificador, "DELETE_SEGMENT")){
        id = DELETE_SEGMENT;
    }
    else if (string_equals_ignore_case(identificador, "EXIT")){
        id = EXIT;
    }
    else if (string_equals_ignore_case(identificador, "YIELD")){
        id = YIELD;
    }
    return id;
}

void add_param_to_instruction(t_list *parametros, t_instruccion *instruccion){
    int valor = instruccion -> cant_parametros;
    int i = 0;
    if(parametros != NULL){
        while(i < instruccion -> cant_parametros && i < list_size(parametros)){
            instruccion -> parametros[i] = list_get(parametros, i);
            i++;
        }
    }
}

t_list *new_list_instruction(void){
    t_list *tmp = list_create();
    return tmp;
}

t_instruccion *new_instruction(t_identificador identificador, t_list* parametros){
    t_instruccion *tmp = malloc(sizeof(t_instruccion));

    tmp->identificador = identificador;
    if(parametros == NULL){
        tmp -> cant_parametros = 0;
        tmp -> parametros = NULL;
    }else {
        tmp -> cant_parametros = list_size(parametros);
        tmp -> parametros = malloc(sizeof(char*) * tmp -> cant_parametros);
        add_param_to_instruction(parametros, tmp);
    }

    return tmp;
}

t_list *parsear_pseudocodigo(FILE *pseudo_file, t_log *logger_consola){

    t_list *list_instructions = new_list_instruction();

    size_t len = 0;     // tamanio de la linea leida
    size_t read;        // cant de caracteres leido
    char *line = NULL;  // linea leida

    while ((read = getline(&line, &len, pseudo_file)) != -1){ 

        // por cada linea que leo, obtengo los tokens, armo la instruccione con sus parametros y la agrego a la lista
        t_list *lines = list_create();
        char *t = strtok(line, "\n");         // separo la primera linea y almaceno en un char
        char **tokens = string_split(t, " "); // separo los tokens (identificador y parametros) SET AX HOLA

        //Agrego a la lista el identificador
        list_add(lines, (void *)tokens[0]);
        log_debug(logger_consola, "Se agregó el operador %s", tokens[0]);

        //Agrego a la lista los parametros de la instruccion
        int i = 1;

        while (tokens[i] != NULL){
            list_add(lines, (void *)tokens[i]);
            log_debug(logger_consola, "Se agregó el parametro %s", tokens[i]);

            i++;
        }

        t_identificador identificador = mapear_identificador(list_get(lines, 0)); 

        t_instruccion *instruccion = new_instruction(identificador, lines); 
        
        list_add(list_instructions, instruccion);

        list_destroy(lines);
        free(tokens);
        free(t);
    }
    free(line);
    return list_instructions;
}

void destroy_instruccion(t_instruccion* instruccion){
    if(instruccion != NULL){
        for(int i = 0; i < instruccion->cant_parametros; i++){
            free(instruccion->parametros[i]);
        }
        free(instruccion);
    }
}

void destroy_lista_instrucciones(t_list* lista_instrucciones){
    for(int i =0; i<list_size(lista_instrucciones); i++){
    destroy_instruccion(list_get(lista_instrucciones, i));
  }
  list_destroy(lista_instrucciones);
}

uint32_t espacio_de_array_parametros(t_instruccion* instruccion){
	uint32_t espacio = 0; //para que no tenga nada antes de sumar
	for(int i=0; i< instruccion->cant_parametros; i++)
		espacio += string_length(instruccion->parametros[i]) * sizeof(char*);
	return espacio;
}

t_buffer* t_instruccion_create_buffer(t_instruccion* instruccion) {
    t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer ->size = sizeof(t_identificador) //identificador
					+ sizeof(uint32_t) //cant_parametros
					+ espacio_de_array_parametros(instruccion); // el espacio q ocupa el array de parametros

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &instruccion->identificador, sizeof(t_instruccion));
	offset += sizeof(t_instruccion);

	memcpy(stream + offset, &instruccion->cant_parametros, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int i = 0; i < instruccion->cant_parametros; i++)
		memcpy(stream + offset, instruccion->parametros[i], string_length(instruccion->parametros[i])* sizeof(char*));

	buffer->stream = stream;
	for(int i = 0; i < instruccion -> cant_parametros; i++)
		free(instruccion->parametros[i]);
	return buffer;
}


t_list* t_lista_instrucciones_create_buffer(t_list* lista_instrucciones) {

    // Creamos una lista para guardar los buffers de cada instrucción
    t_list* lista_buffers = list_create();

    // Creamos el buffer de cada instrucción y lo agregamos a la lista de buffers
    for (int i = 0; i < list_size(lista_instrucciones); i++) {
        t_instruccion* instruccion = list_get(lista_instrucciones, i);
        t_buffer* buffer = t_instruccion_create_buffer(instruccion);
        list_add(lista_buffers, buffer);
    }

    // Calculamos el tamaño total del stream a enviar
    uint32_t size_total = 0;
    for (int i = 0; i < list_size(lista_buffers); i++) {
        t_buffer* buffer = list_get(lista_buffers, i);
        size_total += sizeof(uint32_t) + buffer->size;
    }

    // Creamos el stream a enviar y copiamos los datos de cada buffer
    void* stream = malloc(size_total);
    int offset = 0;
    for (int i = 0; i < list_size(lista_buffers); i++) {
        t_buffer* buffer = list_get(lista_buffers, i);
        memcpy(stream + offset, &buffer->size, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(stream + offset, buffer->stream, buffer->size);
        offset += buffer->size;
    }
    return lista_buffers;
}


/*bool send_instrucciones2(int socket, t_list* lista_instrucciones, t_log* logger){
    t_list* lista_buffers = t_lista_instrucciones_create_buffer(lista_instrucciones);
    bool res = 1;
        void enviar_buffer2(void* buffer) {
            t_package* paquete = package_create(buffer, INSTRUCCIONES);
            if(!package_send(socket, paquete, logger)){
            res = 0;
        }
        package_destroy(paquete);
    }
    list_iterate(lista_buffers, enviar_buffer2);
    return res;
}*/

bool send_instrucciones2(int socket, t_instruccion* instruccion, t_log* logger){
    t_buffer* buffer = t_instruccion_create_buffer(instruccion);
    t_package* paquete = package_create(buffer, INSTRUCCION);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
}
/*

t_list* t_instrucciones_create_from_buffer(t_buffer* buffer) {
    
    t_list* instrucciones = list_create();
    void* stream = buffer->stream;
    uint32_t offset = 0;
    
    // Leer cada instrucción del stream
    while (offset < buffer->size) {
        uint32_t identificador;
        uint32_t cant_parametros;
        memcpy(&identificador, stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(&cant_parametros, stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Crear la estructura para la instrucción y almacenarla en la lista
        t_instruccion* instruccion = malloc(sizeof(t_instruccion));
        instruccion->identificador = identificador;
        instruccion->cant_parametros = cant_parametros;
        instruccion->parametros = malloc(cant_parametros * sizeof(char*));
        
        // Leer los parámetros de la instrucción
        for (int i = 0; i < cant_parametros; i++) {
            uint32_t size_parametro;
            memcpy(&size_parametro, stream + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);
            
            instruccion->parametros[i] = malloc(size_parametro + 1);
            memcpy(instruccion->parametros[i], stream + offset, size_parametro);
            offset += size_parametro;
            instruccion->parametros[i][size_parametro] = '\0';
        }
        
        list_add(instrucciones, instruccion);
    }
    
    return instrucciones;
}
*/