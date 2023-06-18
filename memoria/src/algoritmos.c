#include "algoritmos.h"

// Retorna la base address para el segmento, si no encuentra retorna -1
uint32_t FIRST_FIT(t_segment *segment)
{
    for (int i = 0; i < free_space_table->size; i++)
    {
        if (!bitarray_get(free_space_table, i))
        {
            int size = get_free_space(i);
            if (size >= segment->size)
            {
                return i;
            }
            i += size - 1;
        }
    }
    return -1;
}

// Retorna la base address para el segmento, si no encuentra retorna -1
uint32_t BEST_FIT(t_segment *segment)
{
    int best_size = -1;
    int best_base = -1;
    for (int i = 0; i < free_space_table->size; i++)
    {
        if (!bitarray_get(free_space_table, i))
        {
            int size = get_free_space(i);
            if (size >= segment->size)
            {
                if (best_size == -1 || size < best_size)
                {
                    best_size = size;
                    best_base = i;
                }
            }
            i += size - 1;
        }
    }
    return best_base;
}

// Retorna la base address para el segmento, si no encuentra retorna -1
uint32_t WORST_FIT(t_segment *segment)
{
    int worst_size = -1;
    int worst_base = -1;
    for (int i = 0; i < free_space_table->size; i++)
    {
        if (!bitarray_get(free_space_table, i))
        {
            int size = get_free_space(i);
            if (size >= segment->size)
            {
                if (worst_size == -1 || size > worst_size)
                {
                    worst_size = size;
                    worst_base = i;
                }
            }
            i += size - 1;
        }
    }
    return worst_base;
}
