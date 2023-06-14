#ifndef MEMORY_VARS_H
#define MEMORY_VARS_H

#include <stdint.h>
#include <stdbool.h>
#include <commons/log.h>
#include "server.h"

typedef struct
{
	char *port;
	size_t memory_size;
	size_t segment_zero_size;
	int segment_quantity;
	int memory_time_delay;
	int compactation_time_delay;
	char *compactation_algorithm;
} t_config_memoria;

typedef struct
{
	int socket;
	int HS_module;
} t_client_connection;

// Loggers
extern t_log *logger_main;
extern t_log *logger_aux;

//Estructuras
extern void* memory_space;
extern t_bitarray* free_space_table;

// Servidor
extern bool accept_connections;
extern pthread_t connection;

// Estructuras
extern void *memory_space;
extern t_bitarray *free_space_table;
extern t_segment *segment_0;
extern t_list *all_segments_tables;

#endif /* MEMORY_VARS_H */