#ifndef TYPES_H_
#define TYPES_H_

#include <stdlib.h>
#include <stdint.h>

#include <commons/temporal.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>

/* TIPO DE DATO */
typedef struct
{
    uint32_t dni;
    uint8_t edad;
    uint32_t pasaporte;
    uint32_t nombre_length;
    char *nombre;
} t_persona;

// CONSOLA
typedef enum
{
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
} t_identificador;

typedef struct
{
    t_identificador identificador;
    uint32_t cant_parametros;
    char **parametros;
} t_instruccion;

// CPU

typedef struct
{
    void *AX;
    void *BX;
    void *CX;
    void *DX;
    void *EAX;
    void *EBX;
    void *ECX;
    void *EDX;
    void *RAX;
    void *RBX;
    void *RCX;
    void *RDX;
} t_registers;

typedef struct
{
    uint32_t pid;
    t_instruccion **instructions;
    uint32_t program_counter;
    t_registers *registers;
} t_pcontexto;

// MEMORIA

typedef struct
{
    uint32_t id;
    uint32_t segment_size;
    uint32_t base_direction_lenght;
    char *base_direction;

} t_segments_table;

// FILESYSTEM

typedef struct
{
    uint32_t block_size;
    uint32_t block_count;

} t_superbloque;

typedef struct
{
    char *nombre_archivo;
    int tamanio_archivo;
    uint32_t puntero_directo;
    uint32_t puntero_indirecto;
} t_fcb;

typedef struct
{
    char **archivos_abiertos;
} t_open_files;

// KERNEL

typedef struct
{
    uint32_t pid;
    t_instruccion **instrucciones;
    uint32_t program_counter;
    t_registers *registers;
    t_segments_table *segments_table;
    double est_sig_rafaga;
    t_temporal *tiempo_llegada_ready;
    t_open_files *open_files_table;
} t_pcb;

#endif /* TYPES_H_ */