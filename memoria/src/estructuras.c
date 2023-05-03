#include "estructuras.h"

int start_memory(char* memory_size){
    int memory_space = malloc(memory_size);

    t_bitarray* bitMap = bitarray_create_with_mode(memory_space,memory_size,MSB_FIRST);

}