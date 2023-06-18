#ifndef BITARRAY_H
#define BITARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    unsigned char *array;
    size_t size;
} t_bitarray;

t_bitarray *bitarray_create(size_t size);
void bitarray_destroy(t_bitarray *bitarray);
void bitarray_set(t_bitarray *bitarray, size_t index, bool value);
bool bitarray_get(t_bitarray *bitarray, size_t index);
void bitarray_print(t_bitarray *bitarray);
int get_free_space(int base);

#endif /* BITARRAY_H */