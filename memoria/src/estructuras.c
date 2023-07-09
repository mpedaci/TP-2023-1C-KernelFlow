#include "estructuras.h"

void start_memory(t_config_memoria *config)
{
    // memory_space = malloc(config->memory_size);
    pthread_mutex_init(&memory_space_mutex, NULL);
    memory_space = calloc(1, config->memory_size);
    all_segments_tables = list_create();
    free_space_table = create_free_space_table(config->memory_size);
    segment_0 = create_segment(0, config->segment_zero_size);
    if (segment_0->base_address == -1 || segment_0 == NULL)
        log_error(logger_aux, "No se pudo crear el segmento 0, memoria insuficiente");
    else
        log_info(logger_aux, "Segmento 0 creado correctamente");
}

void destroy_all_segments(){
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *segments_table = list_get(all_segments_tables, i);
        for (int j = 1; j < list_size(segments_table->segment_list); j++)
        {
            t_segment *segment = list_get(segments_table->segment_list, j);
            free(segment);
        }
    }
}

void destroy_all_segments_tables(){
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *segments_table = list_get(all_segments_tables, i);
        list_destroy(segments_table->segment_list);
        free(segments_table);
    }
}

void end_memory()
{
    // Cuando se elimina una tabla de segmentos, el segmento 0 no se libera porque es compartido.
    destroy_all_segments();
    destroy_all_segments_tables();
    list_destroy(all_segments_tables);
    free(segment_0);
    bitarray_destroy(free_space_table);
    free(memory_space);
    pthread_mutex_destroy(&memory_space_mutex);
}

t_bitarray *create_free_space_table(size_t memory_size)
{
    t_bitarray *free_gaps_table = bitarray_create(memory_size);
    // Setear todos los bits en 0
    bitarray_clean_all(free_gaps_table);

    return free_gaps_table;
}
