#include "memory_vars.h"

t_log *logger_main;
t_log *logger_aux;

t_config_memoria *config;

bool accept_connections;
pthread_t connection;

t_segment *segment_0;
t_list *all_segments_tables;

void *memory_space;

t_bitarray *free_space_table;