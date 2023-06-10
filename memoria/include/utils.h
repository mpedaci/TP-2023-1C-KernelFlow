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
int get_base_adress(t_segment *segment);
void delete_segment(int pid, int id);
t_segment *get_segment_by_id(t_segments_table *segments_table, int id);
t_segments_table *create_segments_table(int pid);
void add_segment_to_table(int pid, t_segment *segment);
t_segments_table *get_segments_table_by_pid(int pid);
void delete_segments_table(void *segments_table);
t_data *create_data(char *base_address, int data_length);
void move_data(void* aux_space, int to, int from, int length);
void bitarray_clean_all(t_bitarray *bitmap);
void compact_memory();

#endif /* UTILS_H */