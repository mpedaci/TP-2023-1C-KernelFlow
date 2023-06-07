#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <pthread.h>
#include "memory_vars.h"

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

t_config_memoria *read_config(char *path);
void end_program(t_log *logger_main, t_log *logger_aux, t_config_memoria *config);
t_segment *create_segment(int id, int size);
t_segment *new_segment(int id, int size);
bool is_malloc_possible(int size);
int malloc_segment(t_segment *segment);
t_segments_table *create_segments_table(int pid);
void add_segment_to_table(t_segments_table *segments_table, t_segment *segment);
void delete_segments_table(t_segments_table *segments_table);
t_data *create_data(char *base_address, int data_length);
void bitarray_clean_all(t_bitarray *bitmap);

#endif /* UTILS_H */