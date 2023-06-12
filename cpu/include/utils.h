#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include "communication.h"
#include "types.h"

typedef struct {
    char* ip_memoria;
    char* puerto_memoria;
    char* puerto_escucha;
    char* tam_max_segmento;
    char* retardo_instruccion;
} t_config_cpu;

t_config_cpu* read_config(char* path, t_log* logger);

void close_program_cpu(t_config_cpu* config, t_registers* registers, int socket_client_memoria, t_log* logger, t_log* logger_aux);

t_registers *init_registers();

void set_registers_zero(t_registers *registers);

void registers_destroy(t_registers *registers);

int get_sizeof_register(char *reg);

#endif