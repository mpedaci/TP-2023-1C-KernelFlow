#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include "utils.h"

void start_memory(size_t memory_size);
t_segments_table *create_segments_table(int id, int segment_size, int base_adress);
t_data *create_data(char* base_adress,int data_length);

#endif /* ESTRUCTURAS_H */