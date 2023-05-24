#include "estructuras.h"

void start_memory(size_t memory_size){
    char* memory_space = malloc(memory_size);

    t_bitarray* bitMap = bitarray_create_with_mode(memory_space,memory_size,MSB_FIRST);
}

t_segments_table* create_segments_table(int id, int segment_size, int base_adress){
    t_segments_table* s_table = malloc(sizeof(t_segments_table));
    
    s_table->id = id;
    s_table->segment_size = segment_size;
    s_table->base_adress = base_adress;
    
    return s_table;
}

t_data *create_data(char* base_adress,int data_length){
    t_data* data = malloc(sizeof(t_data));
    data->value=string_duplicate(base_adress);
    data->value_length=data_length;    
    return data;
}
