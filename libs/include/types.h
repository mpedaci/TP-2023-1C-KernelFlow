#ifndef TYPES_H_
#define TYPES_H_

#include <stdlib.h>
#include <stdint.h>
#include <commons/collections/list.h>

/* TIPO DE DATO */
typedef struct {
    uint32_t dni;
    uint8_t edad;
    uint32_t pasaporte;
    uint32_t nombre_length;
    char* nombre;
} t_persona;

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


#endif /* TYPES_H_ */