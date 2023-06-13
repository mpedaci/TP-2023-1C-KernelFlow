#include "estructuras.h"

void start_memory(t_config_memoria *config)
{
    memory_space = malloc(config->memory_size);

    free_space_table = create_free_space_table(config->memory_size);

    segment_0 = create_segment(0, config->segment_zero_size);
    if (segment_0->base_address == -1)
    {
        log_error(logger_aux, "No se pudo crear el segmento 0, memoria insuficiente");
        return;
    }
    log_info(logger_aux, "Segmento 0 creado correctamente");

    all_segments_tables = list_create();
}

void end_memory()
{
    // Cuando se elimina una tabla de segmentos, el segmento 0 no se libera porque es compartido.
    free(segment_0);
    list_destroy_and_destroy_elements(all_segments_tables, delete_segments_table);
    bitarray_destroy(free_space_table);
    free(memory_space);
}

t_bitarray *create_free_space_table(size_t memory_size)
{
    size_t bit_quantity = memory_size / 8;

    t_bitarray *free_gaps_table = bitarray_create_with_mode(memory_space, bit_quantity, MSB_FIRST);

    // Setear todos los bits en 0
    bitarray_clean_all(free_gaps_table);

    return free_gaps_table;
}