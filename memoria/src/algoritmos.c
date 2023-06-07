#include "algoritmos.h"

// Retorna la base address para el segmento, si no encuentra retorna -1
uint32_t FIRST_FIT(t_segment *segment)
{
    int i = 0;
    uint32_t s_size = segment->size;
    uint32_t fst_size = bitarray_get_max_bit(free_space_table);

    while (i < fst_size)
    {
        if (!bitarray_test_bit(free_space_table, i))
        {
            if (i + s_size < fst_size)
            {
                return i;
            }
            else
            {
                i++;
            }
        }
        else
        {
            i++;
        }
    }

    if ((i + 1) == fst_size)
    {
        return -1;
    }
    else
    {
        return i;
    }
}

// Retorna la base address para el segmento, si no encuentra retorna -1
uint32_t BEST_FIT(t_segment *segment)
{
    uint32_t best_size = 0;
    uint32_t best_pos = 0;
    uint32_t s_size = segment->size;
    uint32_t fst_size = bitarray_get_max_bit(free_space_table);

    int i = 0;
    while (i < fst_size)
    {
        if (!bitarray_test_bit(free_space_table, i))
        {
            int j = i;
            while (!bitarray_test_bit(free_space_table, j) && j < fst_size)
            {
                j++;
            }

            uint32_t current_space_size = j - i;

            if (current_space_size >= s_size && (current_space_size < best_size || best_size == 0))
            {
                best_size = current_space_size;
                best_pos = i;
            }

            i = j;
        }

        i++;
    }

    if (best_size == 0)
    {
        return -1;
    }
    else
    {
        return best_pos;
    }
}

// Retorna la base address para el segmento, si no encuentra retorna -1
uint32_t WORST_FIT(t_segment *segment)
{
    uint32_t best_size = 0;
    uint32_t best_pos = 0;
    uint32_t s_size = segment->size;
    uint32_t fst_size = bitarray_get_max_bit(free_space_table);

    int i = 0;
    while (i < fst_size)
    {
        if (!bitarray_test_bit(free_space_table, i))
        {
            int j = i;
            while (!bitarray_test_bit(free_space_table, j) && j < fst_size)
            {
                j++;
            }

            uint32_t current_space_size = j - i;

            if (current_space_size >= s_size && (current_space_size > best_size || best_size == 0))
            {
                best_size = current_space_size;
                best_pos = i;
            }

            i = j;
        }

        i++;
    }

    if (best_size == 0)
    {
        return -1;
    }
    else
    {
        return best_pos;
    }
}