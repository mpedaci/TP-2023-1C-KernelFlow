#include "compactacion.h"

// COMPACTACION
void compact_memory()
{
    int first_zero = 0, last_one = 0, first_one_after_first_zero = 0;

    free_gaps_info(&first_zero, &last_one, &first_one_after_first_zero);

    // sleep(config->compactation_time_delay/1000);

    while (first_zero < last_one)
    {
        t_segment *segment = get_segment_by_address(first_one_after_first_zero);
        int old_address = segment->base_address;
        bitarray_clean_from_and_how_many(free_space_table, old_address, segment->size);
        segment->base_address = get_base_adress(segment);
        move_data(segment->base_address, old_address, segment->size);
        free_gaps_info(&first_zero, &last_one, &first_one_after_first_zero);
    }
}

void free_gaps_info(int *first_zero, int *last_one, int *first_one_after_first_zero)
{
    (*first_zero) = find_first_zero();
    (*last_one) = find_last_one();
    (*first_one_after_first_zero) = find_first_one_after_first_zero(first_zero);
}

int find_first_zero()
{
    int fst_size = bitarray_get_max_bit(free_space_table);
    for (int i = 0; i < fst_size; i++)
    {
        bool status = bitarray_test_bit(free_space_table, i);
        if (!status)
            return i;
    }
    return -1;
}

int find_last_one()
{
    int fst_size = bitarray_get_max_bit(free_space_table);
    for (int i = fst_size; i > 0; i--)
    {
        bool status = bitarray_test_bit(free_space_table, i);
        if (status)
            return i;
    }
    return -1;
}

int find_first_one_after_first_zero(int *fst_zero)
{
    int fst_size = bitarray_get_max_bit(free_space_table);
    int first_zero = *fst_zero;
    for (int i = first_zero; i < fst_size; i++)
    {
        bool status = bitarray_test_bit(free_space_table, i);
        if (status)
            return i;
    }
    return -1;
}