#include "compactacion.h"

// COMPACTACION
void compact_memory()
{
    int first_zero, last_one, first_one_after_first_zero;

    log_info(logger_main, "ENTRO EN FREE GAPS INFO");

    free_gaps_info(&first_zero, &last_one, &first_one_after_first_zero);

    log_info(logger_main, "SALGO DE FREE GAPS INFO");

    //sleep(config->compactation_time_delay/1000);

    while (first_zero < last_one)
    {
        log_info(logger_aux, "SE EJECUTA EL CICLO OTRA VEZ LPM");
        t_segment *segment = get_segment_by_address(first_one_after_first_zero);
        int old_address = segment->base_address;
        bitarray_clean_from_and_how_many(free_space_table, old_address, segment->size);
        segment->base_address = get_base_adress(segment);
        move_data(segment->base_address, old_address, segment->size);
        free_gaps_info(&first_zero, &last_one, &first_one_after_first_zero);
    }
    log_info(logger_main, "Se finalizo el proceso de compactacion");
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
        if (!bitarray_test_bit(free_space_table, i))
        {
            return i;
        }
    }
    return -1;
}

int find_last_one()
{
    int fst_size = bitarray_get_max_bit(free_space_table);

    for (int i = fst_size; i > 0; i--)
    {
        if (bitarray_test_bit(free_space_table, i))
        {
            return i;
        }
    }
    return -1;
}

int find_first_one_after_first_zero(int *fst_zero)
{
    int fst_size = bitarray_get_max_bit(free_space_table);
    int first_zero = *fst_zero;
    printf("first_zero: %d, fst_zero: %d \n", first_zero, *fst_zero);

    for (int i = first_zero; i < fst_size; i++)
    {
        if (bitarray_test_bit(free_space_table, i))
        {
            return i;
        }
    }
    return -1;
}