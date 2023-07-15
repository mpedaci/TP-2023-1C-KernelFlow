#ifndef TYPES_H_
#define TYPES_H_

#include <stdlib.h>
#include <stdint.h>

#include <commons/temporal.h>
#include <commons/collections/list.h>

/* TIPO DE DATO */
typedef enum
{
    SEGMENTATION_FAULT,
    COMPACTATION_REQUIRED,
    OUT_OF_MEMORY,
    MAX_SEG_QUANTITY_REACHED,
    FILE_OPEN,
    FILE_NOT_EXIST,
    FILE_CREATED,
    FILE_READ,
    FILE_TRUNCATED,
    FILE_WRITTEN,
    FILE_NOT_EXISTS,
    SUCCESS,
    PROCESS_NEW,
    PROCESS_END,
    ERROR,
    INVALID_RESOURCE,
    PROCESS_ABORTED
} t_status_code;

// CONSOLA
typedef enum
{
    I_F_READ,
    I_F_WRITE,
    I_SET,
    I_MOV_IN,
    I_MOV_OUT,
    I_F_TRUNCATE,
    I_F_SEEK,
    I_CREATE_SEGMENT,
    I_I_O,
    I_WAIT,
    I_SIGNAL,
    I_F_OPEN,
    I_F_CLOSE,
    I_DELETE_SEGMENT,
    I_EXIT,
    I_YIELD
} t_identificador;

typedef struct
{
    t_identificador identificador; // uint32_t
    uint32_t cant_parametros;
    uint32_t p_length[4]; // 4 * 4
    t_list *parametros;
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
    t_list *instructions;
    uint32_t program_counter;
    t_registers *registers;
    t_list *segments;
} t_pcontexto;

typedef struct
{
    uint32_t pid;
    t_list *instructions;
    uint32_t program_counter;
    t_registers *registers;
    t_instruccion *motivo_desalojo;
    t_status_code status_code;
} t_pcontexto_desalojo;

// MEMORIA

typedef struct
{
    uint32_t id;
    uint32_t size;
    uint32_t base_address;
} t_segment;

typedef struct
{
    uint32_t pid;
    t_list *segment_list;
} t_segments_table;

typedef struct
{
    uint32_t pid;
    t_instruccion *instruccion;
} t_pid_instruccion;
typedef struct
{
    uint32_t value_length;
    char *value;
} t_data;

typedef uint32_t t_address;

typedef struct {
    void *data;
    uint32_t size;
} t_info;

typedef struct
{
    uint32_t base_address;
    t_info *info;
} t_info_write;

typedef struct
{
    uint32_t base_address;
    uint32_t size;
} t_info_read;

// FILESYSTEM

typedef struct
{
    uint32_t block_size;
    uint32_t block_count;
} t_superbloque;

typedef struct
{
    char *nombre_archivo;
    uint32_t tamanio_archivo;
    uint32_t puntero_directo;
    uint32_t puntero_indirecto;
} t_fcb;

typedef struct
{
    char *nombre_archivo;
    uint32_t puntero_archivo;
} t_open_files;

// KERNEL
typedef enum
{
    QNEW,
    QREADY,
    QEXEC,
    QBLOCK,
    QEXIT
} t_queue_id;

typedef struct
{
    uint32_t pid;
    t_list *instrucciones;
    uint32_t program_counter;
    t_registers *registers;
    t_segments_table *segments_table;
    double est_sig_rafaga;
    t_temporal *tiempo_llegada_ready;
    t_temporal *tiempo_entrada_cpu;
    t_temporal *tiempo_salida_cpu;
    t_list *open_files_table;
    t_list *shared_resources;
    t_status_code exit_status;
    t_queue_id next_queue;
} t_pcb;

typedef struct
{
    uint32_t pid;
    t_status_code status;
} t_pid_status;


#endif /* TYPES_H_ */