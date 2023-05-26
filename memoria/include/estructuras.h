#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include "utils.h"

void start_memory(t_config_memoria* config);
void end_memory();
t_bitarray* create_free_space_table(size_t memory_size);
t_segment* create_segment(int id, int segment_size,int base_adress);
t_segments_table* create_segments_table(int pid);
void add_segment_to_table(t_segments_table* segments_table,t_segment* segment);
void delete_segments_table(t_segments_table* segments_table);
t_data *create_data(char* base_adress,int data_length);
void bitarray_clean_all(t_bitarray* bitmap);

#endif /* ESTRUCTURAS_H */