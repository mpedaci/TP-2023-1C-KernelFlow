#include "parser.h"

t_identificador mapear_identificador(char *identificador)
{
    t_identificador id;
    if (string_equals_ignore_case(identificador, "F_READ"))
        id = I_F_READ;
    else if (string_equals_ignore_case(identificador, "F_WRITE"))
        id = I_F_WRITE;
    else if (string_equals_ignore_case(identificador, "SET"))
        id = I_SET;
    else if (string_equals_ignore_case(identificador, "MOV_IN"))
        id = I_MOV_IN;
    else if (string_equals_ignore_case(identificador, "MOV_OUT"))
        id = I_MOV_OUT;
    else if (string_equals_ignore_case(identificador, "F_TRUNCATE"))
        id = I_F_TRUNCATE;
    else if (string_equals_ignore_case(identificador, "F_SEEK"))
        id = I_F_SEEK;
    else if (string_equals_ignore_case(identificador, "CREATE_SEGMENT"))
        id = I_CREATE_SEGMENT;
    else if (string_equals_ignore_case(identificador, "I/O"))
        id = I_I_O;
    else if (string_equals_ignore_case(identificador, "WAIT"))
        id = I_WAIT;
    else if (string_equals_ignore_case(identificador, "SIGNAL"))
        id = I_SIGNAL;
    else if (string_equals_ignore_case(identificador, "F_OPEN"))
        id = I_F_OPEN;
    else if (string_equals_ignore_case(identificador, "F_CLOSE"))
        id = I_F_CLOSE;
    else if (string_equals_ignore_case(identificador, "DELETE_SEGMENT"))
        id = I_DELETE_SEGMENT;
    else if (string_equals_ignore_case(identificador, "EXIT"))
        id = I_EXIT;
    else if (string_equals_ignore_case(identificador, "YIELD"))
        id = I_YIELD;
    return id;
}

void add_param_size_to_instruction(t_list *parametros, t_instruccion *instruccion)
{
    int i = 0;
    while (i < instruccion->cant_parametros)
    {
        char *param = list_get(parametros, i);
        instruccion->p_length[i] = strlen(param) + 1;
        i++;
    }
}

t_instruccion *new_instruction(t_identificador identificador, t_list *parametros)
{
    t_instruccion *tmp = malloc(sizeof(t_instruccion));
    tmp->identificador = identificador;
    tmp->cant_parametros = list_size(parametros);
    tmp->parametros = parametros;
    for (size_t i = 0; i < 4; i++)
        tmp->p_length[i] = 0;
    add_param_size_to_instruction(parametros, tmp);
    return tmp;
}

t_instruccion *process_line(char *line)
{
    // por cada linea que leo, obtengo los tokens, armo la instruccione con sus parametros y la agrego a la lista
    char *t = strtok(line, "\n");         // separo la primera linea y almaceno en un char
    char **tokens = string_split(t, " "); // separo los tokens (identificador y parametros) SET AX HOLA
    // Genero el identificador de la instruccion
    char *identificador = tokens[0];
    t_identificador id = mapear_identificador(identificador);
    // Agrego a la lista los parametros de la instruccion
    t_list *parameters = list_create();
    int i = 1; // A partir de 1 son parametros - La lista puede estar vacia
    while (tokens[i] != NULL)
    {
        list_add(parameters, (void *)tokens[i]);
        i++;
    }
    t_instruccion *instruccion = new_instruction(id, parameters);
    free(identificador);
    free(tokens);
    return instruccion;
}

t_list *parsear_pseudocodigo(FILE *pseudo_file, t_log *logger_consola)
{
    t_list *list_instructions = list_create();
    size_t len = 0;    // tamanio de la linea leida - USADAS PARA GETLINE
    size_t read;       // cant de caracteres leido - USADAS PARA GETLINE
    char *line = NULL; // linea leida
    while ((read = getline(&line, &len, pseudo_file)) != -1)
    {
        t_instruccion *instruccion = process_line(line);
        list_add(list_instructions, instruccion);
    }
    free(line);
    return list_instructions;
}