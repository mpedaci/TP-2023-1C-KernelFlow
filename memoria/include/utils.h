#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <pthread.h>
#include "server.h"
#include "modulos.h"

typedef struct
{
    char* port;
	char* memory_size;
	char* segment_zero_size;
	char* segment_quantity;
	char* memory_time_delay;
	char* compactation_time_delay;
	char* compactation_algorithm;
} t_config_memoria;

t_config_memoria *read_config(char *path, t_log *logger);
void end_program(t_log *logger_main, t_log *logger_aux, t_config_memoria *config);

#endif /* UTILS_H */