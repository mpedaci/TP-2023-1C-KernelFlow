#include "parser.h"

t_identificador mapear_identificador(char *identificador){

    t_identificador id;

    if (string_equals_ignore_case(identificador, "F_READ"))
    {
        id = F_READ;
    }
    else if (string_equals_ignore_case(identificador, "F_WRITE"))
    {
        id = F_WRITE;
    }
    else if (string_equals_ignore_case(identificador, "SET"))
    {
        id = SET;
    }
    else if (string_equals_ignore_case(identificador, "MOV_IN"))
    {
        id = MOV_IN;
    }
    else if (string_equals_ignore_case(identificador, "MOV_OUT"))
    {
        id = MOV_OUT;
    }
    else if (string_equals_ignore_case(identificador, "F_TRUNCATE"))
    {
        id = F_TRUNCATE;
    }
    else if (string_equals_ignore_case(identificador, "F_SEEK"))
    {
        id = F_SEEK;
    }
    else if (string_equals_ignore_case(identificador, "CREATE_SEGMENT"))
    {
        id = CREATE_SEGMENT;
    }
    else if (string_equals_ignore_case(identificador, "I_O"))
    {
        id = I_O;
    }
    else if (string_equals_ignore_case(identificador, "WAIT"))
    {
        id = WAIT;
    }
    else if (string_equals_ignore_case(identificador, "SIGNAL"))
    {
        id = SIGNAL;
    }
    else if (string_equals_ignore_case(identificador, "F_OPEN"))
    {
        id = F_OPEN;
    }
    else if (string_equals_ignore_case(identificador, "F_CLOSE"))
    {
        id = F_CLOSE;
    }
    else if (string_equals_ignore_case(identificador, "DELETE_SEGMENT"))
    {
        id = DELETE_SEGMENT;
    }
    else if (string_equals_ignore_case(identificador, "EXIT"))
    {
        id = EXIT;
    }
    else if (string_equals_ignore_case(identificador, "YIELD"))
    {
        id = YIELD;
    }
    return id;
}

void add_param(t_identificador identificador, t_instruccion *instruccion, t_list *parametros, t_log *logger_console){

    switch (identificador){
    // instruccion que lleva 3 parametros
    case F_READ:
    case F_WRITE:
        add_param_to_instruction(instruccion, list_get(parametros, 1)); // Retorna el contenido de una posicion determianda de la lista
        add_param_to_instruction(instruccion, list_get(parametros, 2));
        add_param_to_instruction(instruccion, list_get(parametros, 3));
        break;

    // instruccion que lleva 2 parametros
    case SET:
    case MOV_IN:
    case MOV_OUT:
    case F_TRUNCATE:
    case F_SEEK:
    case CREATE_SEGMENT:
        add_param_to_instruction(instruccion, list_get(parametros, 1)); // Retorna el contenido de una posicion determianda de la lista, lo casteo a (int) para definir tipo
        add_param_to_instruction(instruccion, list_get(parametros, 2));
        break;

    // instruccion que lleva 1 parametros
    case I_O:
    case WAIT:
    case SIGNAL:
    case F_OPEN:
    case F_CLOSE:
    case DELETE_SEGMENT:
        add_param_to_instruction(instruccion, list_get(parametros, 1));
        break;

    // instruccion que no lleva parametros
    case EXIT:
    case YIELD:
        break;
    }
}

void add_param_to_instruction(t_instruccion *instruccion, char *parametro){ 
    list_add(instruccion->parametros, parametro);
}

t_lista_instrucciones *new_list_instruction(void){
    t_lista_instrucciones *tmp = list_create();
    return tmp;
}

t_instruccion *new_instruction(t_identificador identificador){
    t_instruccion *tmp = malloc(sizeof(t_instruccion));

    tmp->identificador = identificador;
    tmp->parametros = list_create();

    return tmp;
}

t_lista_instrucciones *parsear_pseudocodigo(FILE *pseudo_file, t_log *logger_consola){

    t_lista_instrucciones *list_instructions = new_list_instruction();

    size_t len = 0;     // tamanio de la linea leida
    size_t read;        // cant de caracteres leido
    char *line = NULL;  // linea leida

    while ((read = getline(&line, &len, pseudo_file)) != -1){ 
        // lee una linea entera de pseudofile, almacena el texto en line y devuelve un nro de caracteres leidos sin contar el /o (si hay un error devuelve -1)

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

        t_instruccion *instruccion = new_instruction(identificador); 
        add_param(identificador, instruccion, lines, logger_consola);
        list_add(list_instructions, instruccion);
    }

    return list_instructions;
}