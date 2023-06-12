#ifndef MEMORY_VARS_H
#define MEMORY_VARS_H

#include <commons/log.h>
#include <commons/config.h>
#include "server.h"

typedef struct{
	char *port;
	size_t memory_size;
	size_t segment_zero_size;
	int segment_quantity;
	int memory_time_delay;
	int compactation_time_delay;
	char *compactation_algorithm;
} t_config_memoria;

// Loggers
extern t_log *logger_main;
extern t_log *logger_aux;

// Config
extern t_config_memoria *config;

// Estructuras
extern void *memory_space;
extern t_bitarray *free_space_table;

extern t_segment *segment_0;
extern t_list *all_segments_tables;

#endif /* MEMORY_VARS_H */