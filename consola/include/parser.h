#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "package.h"

#include <commons/string.h>
#include <commons/txt.h>
#include <commons/log.h>
#include <commons/collections/list.h>

typedef enum {
    F_READ ,
    F_WRITE,
    SET ,
    MOV_IN ,
    MOV_OUT ,
    F_TRUNCATE,
    F_SEEK ,
    CREATE_SEGMENT,
    I_O ,
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
    uint32_t cant_parametros;
    char** parametros;
}t_instruccion;

//void enviar_instrucciones_por_socket(int socket, t_list* lista_instrucciones); 

t_identificador mapear_identificador(char *identificador);
void add_param_to_instruction(t_list *parametros, t_instruccion *instruccion);
t_list *new_list_instruction(void);
t_instruccion *new_instruction(t_identificador identificador, t_list* parametros);
t_list *parsear_pseudocodigo(FILE *pseudo_file, t_log *logger_consola);
void destroy_instruccion(t_instruccion* instruccion);
void destroy_lista_instrucciones(t_list* lista_instrucciones);
bool send_instrucciones2(int socket, t_instruccion* instruccion, t_log* logger);
//bool send_instrucciones2(int socket, t_list* lista_instrucciones, t_log* logger);

