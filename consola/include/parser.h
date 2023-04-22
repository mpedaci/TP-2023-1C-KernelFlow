/*#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <commons/collections/list.h>


// cada linea tiene el formato de INSTRUCCION PARAMETROS
//enum de las posibles instrucciones que va a parsear 
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
}ID_INSTRUCCION;

#define IDENTIFICADORES_VALIDOS "F_READ, F_WRITE, SET, MOV_IN, MOV_OUT, F_TRUNCATE, F_SEEK, CREATE_SEGMENT, I_O, WAIT, SIGNAL, F_OPEN, F_CLOSE, DELETE_SEGMENT, EXIT, YIELD"

bool el_proceso_es_valido(FILE* instrucciones);
bool es_una_instruccion_valida(char** instruccion);
bool tiene_los_parametros_correctos(char** instruccion);
int parametros_segun_id(char* identificador);
int cantidad_de_parametros(ID_INSTRUCCION identificador);
ID_INSTRUCCION get_id(char* identificador);*/
