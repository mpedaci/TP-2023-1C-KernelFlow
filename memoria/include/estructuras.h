#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include "utils.h"

int start_memory(char* memory_size);
t_segments_table *create_segments_table(int id, int segment_size, int base_adress);
t_data *create_data(int base_adress,int data_length);

#endif /* ESTRUCTURAS_H */