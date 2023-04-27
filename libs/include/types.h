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

typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCK,
    EXIT,
} process_state;

typedef struct {
    uint32_t pib;
    t_list* instructions; // por ahora es una lista de strings, puede cambiar
    uint32_t program_counter;
    t_registers* registers;
} t_pcontexto;

#endif /* TYPES_H_ */