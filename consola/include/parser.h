#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <commons/log.h>
#include <commons/collections/list.h>

typedef enum {
    F_READ,
    F_WRITE,
    SET,
    MOV_IN,
    MOV_OUT,
    F_TRUNCATE,
    F_SEEK,
    CREATE_SEGMENT,
    I_O,
    WAIT,
    SIGNAL,
    F_OPEN,
    F_CLOSE,
    DELETE_SEGMENT,
    EXIT,
    YIELD
}t_identificador;

typedef struct{
    t_identificador identificador;
    t_list* parametros;
}t_instruccion;

typedef t_list t_lista_instrucciones;

typedef struct{
    //size_t tamanio;       Agrego el tamanio porque le sirve a juan en CPU pero no se si lo voy a implementar
    t_lista_instrucciones* lista_instrucciones;
}t_proceso;


t_identificador mapear_identificador(char *identificador);
void add_param(t_identificador identificador, t_instruccion *instruccion, t_list *parametros, t_log *logger_console);
void add_param_to_instruction(t_instruccion *instruccion, char *parametro);
t_lista_instrucciones *new_list_instruction(void);
t_instruccion *new_instruction(t_identificador identificador);
t_lista_instrucciones *parsear_pseudocodigo(FILE *pseudo_file, t_log *logger_consola);





