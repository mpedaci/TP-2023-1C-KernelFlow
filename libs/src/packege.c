#include "package.h"

/* BUFFERS TIPOS DE DATOS -> SEND */

uint32_t espacio_de_array_parametros(t_instruccion* instruccion){
	uint32_t espacio = 0; //para que no tenga nada antes de sumar
	for(int i=0; i< instruccion->cant_parametros; i++)
		espacio += strlen(instruccion->parametros[i]) * sizeof(char*);
	return espacio;
}

t_buffer* t_lista_instrucciones_create_buffer(t_list* lista_instrucciones){

     t_buffer* buffer = malloc(sizeof(t_buffer));
     
    uint32_t size_total = 0;

    for(int i = 0; i < list_size(lista_instrucciones); i++){
        t_instruccion* instruccion = list_get(lista_instrucciones, i);
        size_total += sizeof(t_identificador) + sizeof(uint32_t) + 
        espacio_de_array_parametros(instruccion);
    }

    void* stream = malloc(size_total);
    //Desplazamiento
    int offset=0;

    for(int i = 0; i < list_size(lista_instrucciones); i++){
        t_instruccion* instruccion = list_get(lista_instrucciones, i);

        memcpy(stream + offset, &instruccion -> identificador, sizeof(t_instruccion));
        offset += sizeof(t_instruccion);

        memcpy(stream + offset, &instruccion->cant_parametros, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        for(int i = 0; i< instruccion->cant_parametros; i++)
        memcpy(stream + offset, instruccion->parametros, strlen(instruccion->parametros[i]) * sizeof(char*));
    }
    buffer -> stream = stream;
    return buffer;
}

t_buffer* t_persona_create_buffer(t_persona persona)
{
    t_buffer* buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t) * 3         // DNI, Pasaporte y longitud del nombre
                 + sizeof(uint8_t)              // Edad
                 + strlen(persona.nombre) + 1;  // La longitud del string nombre. Le sumamos 1 para enviar tambien el caracter centinela '\0'. Esto se podría obviar, pero entonces deberíamos agregar el centinela en el receptor.

    void* stream = malloc(buffer->size);
    // Desplazamiento
    int offset = 0;

    // ACA SE DEFINE EL STRUCT Y SUS CAMPOS A GUARDAR
    // TENER EN CUENTA LOS TIPOS DE DATOS
    memcpy(stream + offset, &persona.dni, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &persona.edad, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &persona.pasaporte, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    // Para el nombre primero mandamos el tamaño y luego el texto en sí:
    memcpy(stream + offset, &persona.nombre_length, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, persona.nombre, strlen(persona.nombre) + 1);

    // No tiene sentido seguir calculando el desplazamiento, ya ocupamos el buffer completo
    buffer->stream = stream;
    return buffer;
}

t_buffer* null_buffer(){
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = 0;
    buffer->stream = NULL;
    return buffer;
}

/* BUFFERS TIPOS DE DATOS -> RECV */

t_list* t_lista_instrucciones_create_from_buffer(t_buffer* buffer){
    t_list* lista_instrucciones = malloc(sizeof(t_list));
    void* stream = buffer->stream;
    uint32_t offset = 0;
        
        while (offset < buffer->size){
        t_identificador identificador;
        uint32_t cant_parametros;
        memcpy(&identificador, stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(&cant_parametros, stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        // Crear la estructura para la instrucción y almacenarla en la lista
        t_instruccion *instruccion = malloc(sizeof(t_instruccion));
        instruccion->identificador = identificador;
        instruccion->cant_parametros = cant_parametros;
        instruccion->parametros = malloc(cant_parametros * sizeof(char *));

        // Leer los parámetros de la instrucción
        for (int i = 0; i < cant_parametros; i++)
        {
            uint32_t size_parametro;
            memcpy(&size_parametro, stream + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);

            instruccion->parametros[i] = malloc(size_parametro + 1);
            memcpy(instruccion->parametros[i], stream + offset, size_parametro);
            offset += size_parametro;
            instruccion->parametros[i][size_parametro] = '\0';
        }

        list_add(lista_instrucciones, instruccion);
    }

    return lista_instrucciones;
}

t_persona* t_persona_create_from_buffer(t_buffer* buffer)
{
    t_persona* persona = malloc(sizeof(t_persona));
    void* stream = buffer->stream;

    // ACA SE DEFINE EL STRUCT Y SUS CAMPOS A GUARDAR
    // TENER EN CUENTA LOS TIPOS DE DATOS
    // Deserializamos los campos que tenemos en el buffer
    memcpy(&(persona->dni), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(persona->edad), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    memcpy(&(persona->pasaporte), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    // Por último, para obtener el nombre, primero recibimos el tamaño y luego el texto en sí:
    memcpy(&(persona->nombre_length), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    persona->nombre = malloc(persona->nombre_length);
    memcpy(persona->nombre, stream, persona->nombre_length);

    return persona;
}

/* PAQUETES */

t_package* package_create(t_buffer *buffer, int operation_code)
{
    t_package* paquete = malloc(sizeof(t_package));
    paquete->operation_code = operation_code;
    if (buffer == NULL)
        paquete->buffer = null_buffer();
    else
        paquete->buffer = buffer;
    return paquete;
}

uint32_t package_get_size(t_package* paquete)
{
    return paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t);
}

bool package_send(int socket, t_package* paquete, t_log* logger)
{
    // Armamos el stream a enviar
    // Buffer Size + Size of Operation Code Var + Size of Buffer Size Var
    void* a_enviar = malloc(paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
    int offset = 0;
    memcpy(a_enviar + offset, &(paquete->operation_code), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
    if (send(socket, a_enviar, paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0) == -1){
        log_error(logger, "Error al enviar el paquete");
        free(a_enviar);
        return false;
    }
    printf("Enviado paquete con codigo de operacion: %d\n", paquete->operation_code);
    free(a_enviar);
    return true;
}

t_package* package_recv(int socket, t_log* logger){
    t_package* paquete = malloc(sizeof(t_package));
    paquete->buffer = malloc(sizeof(t_buffer));

    // Primero recibimos el codigo de operacion
    recv(socket, &(paquete->operation_code), sizeof(uint8_t), 0);
    log_info(logger, "Recibido codigo de operacion: %d", paquete->operation_code);
    // Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
    recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);
    
    return paquete;
}

void package_destroy(t_package* paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}