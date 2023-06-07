#include "algoritmos.h"


// Retorna un puntero a la posicion del primer bloque de memoria que cumpla con el tamaño del segmento, si no encuentra retorna NULL
void *FIRST_FIT(uint32_t size) {
    void *pos = memory_space;

    int i = 0;
    while(free_space_table->bitarray[i] == "1" && i < free_space_table->size) {
        i++;
    }

    if(i == free_space_table->size) {
        return NULL;
    }

    pos += i;

    return pos;
}

// Retorna un puntero a la posicion del bloque de memoria que cumpla con el tamaño del segmento y sel bloque sea el menor posible, si no encuentra retorna NULL
void *BEST_FIT(uint32_t size) {
    void *pos = memory_space;
    uint32_t best_size = 0;
    void *best_pos = NULL;
    
    int i = 0;
    while(i < free_space_table->size) {
        if(free_space_table->bitarray[i] == "0") {
            int j = i;
            while(free_space_table->bitarray[j] == "0" && j < free_space_table->size) {
                j++;
            }

            uint32_t current_space_size = j - i;

            if(current_space_size >= size && (current_space_size < best_size || best_size == 0)) {
                best_size = current_space_size;
                best_pos = pos + i;
            }

            i = j;
        }

        i++;
    }

    return best_pos;
}

// Retorna un puntero a la posicion del bloque de memoria que cumpla con el tamaño del segmento y que el bloque sea el mayor, si no encuentra retorna NULL
void *WORST_FIT(uint32_t size) {
    void *pos = NULL;
    uint32_t best_size = 0;
    void *best_pos = NULL;
    
    int i = 0;
    while(i < free_space_table->size) {
        if(free_space_table->bitarray[i] == "0") {
            int j = i;
            while(free_space_table->bitarray[j] == "0" && j < free_space_table->size) {
                j++;
            }

            uint32_t current_space_size = j - i;

            if(current_space_size >= size && (current_space_size > best_size || best_size == 0)) {
                best_size = current_space_size;
                best_pos = pos + i;
            }

            i = j;
        }

        i++;
    }

    return best_pos;
}