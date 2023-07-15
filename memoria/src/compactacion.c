#include "compactacion.h"

int find_first_zero()
{
    for (int i = 0; i < free_space_table->size; i++)
    {
        if (!bitarray_get(free_space_table, i))
        {
            return i;
        }
    }
    return -1;
}

int find_last_one()
{
    for (int i = free_space_table->size - 1; i > 0; i--)
    {
        if (bitarray_get(free_space_table, i))
        {
            return i;
        }
    }
    return -1;
}

int find_first_one_after_first_zero(int fst_zero)
{
    for (int i = fst_zero; i < free_space_table->size; i++)
    {
        if (bitarray_get(free_space_table, i))
        {
            return i;
        }
    }
    return -1;
}

// COMPACTACION
void compact_memory()
{
    log_info(logger_aux, "Iniciando compactacion");
    sleep(config->compactation_time_delay/1000);
    int first_zero = 0, last_one = 0, first_one_after_first_zero = 0;
    first_zero = find_first_zero();
    last_one = find_last_one();
    first_one_after_first_zero = find_first_one_after_first_zero(first_zero);
    while (first_zero < last_one)
    {
        t_segment *segment = get_segment_by_address(first_one_after_first_zero);
        int old_address = segment->base_address;

        for (int i = segment->base_address; i < (segment->base_address + segment->size); i++)
            bitarray_set(free_space_table, i, false);

        segment->base_address = get_base_adress(segment);

        // Mueve la informacion
        move_data(segment->base_address, old_address, segment->size);
        // Recalcula los comparadores
        first_zero = find_first_zero();
        last_one = find_last_one();
        first_one_after_first_zero = find_first_one_after_first_zero(first_zero);
    }

    log_info(logger_aux, "Compactacion finalizada");
}
