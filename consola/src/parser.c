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
