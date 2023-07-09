#ifndef MEMORY_VARS_H
#define MEMORY_VARS_H

#include <pthread.h>

#include <commons/log.h>
#include <commons/config.h>
#include "bitarray.h"

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

// Config
extern t_config_memoria *config;

// Estado del programa
extern bool end_program_flag;

// Servidor
extern int server_socket;
extern bool accept_connections;
extern pthread_t thr_server;
extern pthread_t thr_server_conn;

extern bool fs_connected;
extern bool cpu_connected;
extern bool kernel_connected;

// Estructuras
extern void *memory_space;
extern pthread_mutex_t memory_space_mutex;
extern t_bitarray *free_space_table;
extern t_segment *segment_0;
extern t_list *all_segments_tables;

#endif /* MEMORY_VARS_H */