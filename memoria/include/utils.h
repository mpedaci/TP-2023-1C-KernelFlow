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
#include "bitarray.h"

t_config_memoria *read_config(char *path);
void end_program(t_log *logger_main, t_log *logger_aux, t_config_memoria *config);

t_data *create_data(char *base_address, int data_length);
void move_data(int to, int from, int length);

t_segment *get_segment_by_id(int id);
t_segment *get_segment_by_address(int address);
t_segments_table *get_segments_table_by_pid(int pid);

void bitarray_clean_from_and_how_many(t_bitarray *bitmap, int from, int how_many);
void bitarray_clean_all(t_bitarray *bitmap);
int get_free_space(int base);

t_info *read_memory(int base_address, int size);
bool write_memory(int base_address, int size, void *data);
void move_data(int to, int from, int length);

void print_all_segments_tables();

int get_pid_by_address(int address);

#endif /* UTILS_H */