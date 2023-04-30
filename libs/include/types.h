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

typedef struct {
    void* AX;
    void* BX;
    void* CX;
    void* DX;
    void* EAX;
    void* EBX;
    void* ECX;
    void* EDX;
    void* RAX;
    void* RBX;
    void* RCX;
    void* RDX;
} t_registers;

typedef struct {
    uint32_t pid;
    t_list* instructions;
    uint32_t program_counter;
    t_registers* registers;
} t_pcontexto;

typedef enum {
    I_F_READ ,
    I_F_WRITE,
    I_SET ,
    I_MOV_IN ,
    I_MOV_OUT ,
    I_F_TRUNCATE,
    I_F_SEEK ,
    I_CREATE_SEGMENT,
    I_I_O ,
    I_WAIT,
    I_SIGNAL,
    I_F_OPEN,
    I_F_CLOSE,
    I_DELETE_SEGMENT,
    I_EXIT,
    I_YIELD 
}t_identificador;

typedef struct{
    t_identificador identificador;
    uint32_t cant_parametros;
    char** parametros;
}t_instruccion;

#endif /* TYPES_H_ */