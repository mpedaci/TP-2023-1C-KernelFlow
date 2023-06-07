#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <commons/string.h>
#include <pthread.h>
#include "memory_vars.h"
#include "algoritmos.h"

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