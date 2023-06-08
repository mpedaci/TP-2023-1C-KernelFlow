#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/string.h>
#include <commons/txt.h>
#include <commons/log.h>
#include <commons/collections/list.h>

#include "package.h"
#include "communication.h"

t_identificador mapear_identificador(char *identificador);
void add_param_size_to_instruction(t_list *parametros, t_instruccion *instruccion);
t_instruccion *new_instruction(t_identificador identificador, t_list *parametros);
t_instruccion *process_line(char *line);
t_list *parsear_pseudocodigo(FILE *pseudo_file, t_log *logger_consola);
void destroy_instruccion(t_instruccion *instruccion);
void destroy_lista_instrucciones(t_list *lista_instrucciones);