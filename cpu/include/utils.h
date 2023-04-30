#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct {
    char* ip_memoria;
    char* puerto_memoria;
    char* puerto_escucha;
    char* tam_max_segmento;
    char* retardo_instruccion;
} t_config_cpu;

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
    int pib;
    char** instructions; // por ahora es una lista de strings, puede cambiar
    int program_counter;
    t_registers registers;
    // tablas de segmento
    // tabla de archivos
} t_execution_context; // hablar con equipo para ver este tema ##################################

typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCK,
    EXIT,
} process_state;


t_config_cpu* read_config(char* path, t_log* logger);

void close_program_cpu(t_config_cpu* config, t_registers* registers, t_log* logger, t_log* logger_aux);

t_registers* init_registers();

#endif