#include "estructuras.h"

void start_memory(t_config_memoria* config){
    memory_space = malloc(config->memory_size);
    
    free_space_table = create_free_space_table(config->memory_size);

    all_segments_tables = list_create();

    segment_0 = create_segment(0,config->segment_zero_size,0);
}

void end_memory(){
    //Cuando eliminas una tabla de segmentos, no se libera el segmento 0 porque es compartido.
    free(segment_0);
    list_destroy_and_destroy_elements(all_segments_tables,free);
    bitarray_destroy(free_space_table);
    free(memory_space);
}


t_bitarray* create_free_space_table(size_t memory_size){
    size_t bit_quantity = memory_size/8;

    t_bitarray* free_gaps_table = bitarray_create_with_mode(memory_space,bit_quantity,MSB_FIRST);

    //Se establecen todos los bits en 0
    bitarray_clean_all(free_gaps_table);

    return free_gaps_table;
}

t_segment* create_segment(int id, int size,int base_address){
    t_segment* segment = malloc(sizeof(t_segment));

    segment->id = id;
    segment->size = size;
    segment->base_address = base_address;

    for(int i=base_address;i<(base_address+size);i++){
        bitarray_set_bit(free_space_table,i);
    }

    return segment;    
}


t_segments_table* create_segments_table(int pid){
    t_segments_table* segments_table = malloc(sizeof(t_segments_table));
    segments_table->pid=pid;
    segments_table->segment_list=list_create();

    list_add(segments_table->segment_list,segment_0);

    list_add(all_segments_tables,segments_table);

    return segments_table;
}

//No se si dejarla como void o que devuelva la tabla con el segmento nuevo
void add_segment_to_table(t_segments_table* segments_table,t_segment* segment){
    list_add(segments_table->segment_list,segment);
}

void delete_segments_table(t_segments_table* segments_table){
    //Inicio "i" en 1, ya que en 0 esta el segmento cero
    for (int i = 1; i < list_size(segments_table->segment_list); i++){
        list_remove_and_destroy_element(segments_table->segment_list,i,free);
    }
    list_destroy(segments_table->segment_list);
    free(segments_table);
}

t_data *create_data(char* base_address,int data_length){
    t_data* data = malloc(sizeof(t_data));
    data->value=string_duplicate(base_address);
    data->value_length=data_length;    
    return data;
}

void bitarray_clean_all(t_bitarray* bitmap){
    for(int i=0; i<bitarray_get_max_bit(bitmap);i++){
        bitarray_clean_bit(bitmap,i);
    }
}