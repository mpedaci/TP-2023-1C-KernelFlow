#include "bitarray.h"

// Create a new bit array with the specified size
t_bitarray *bitarray_create(size_t size)
{
    t_bitarray *bitarray = malloc(sizeof(t_bitarray));
    bitarray->size = size;
    bitarray->array = calloc((size + 7) / 8, sizeof(unsigned char));
    return bitarray;
}

// Free the memory used by the bit array
void bitarray_destroy(t_bitarray *bitarray)
{
    free(bitarray->array);
    free(bitarray);
}

// Set the bit at the specified index to 1
void bitarray_set(t_bitarray *bitarray, size_t index, bool value)
{
    if (index >= bitarray->size)
    {
        printf("Index out of bounds.\n");
        return;
    }

    if (value)
        bitarray->array[index / 8] |= (1 << (index % 8));
    else
        bitarray->array[index / 8] &= ~(1 << (index % 8));
}

// Get the value of the bit at the specified index (1 or 0)
bool bitarray_get(t_bitarray *bitarray, size_t index)
{
    if (index >= bitarray->size)
    {
        printf("Index out of bounds.\n");
        return false;
    }

    return (bitarray->array[index / 8] >> (index % 8)) & 1;
}

// Print the bit array as a sequence of 1s and 0s
void bitarray_print(t_bitarray *bitarray)
{
    for (size_t i = 0; i < bitarray->size; i++)
    {
        printf("%d", bitarray_get(bitarray, i));
    }
    printf("\n");
}