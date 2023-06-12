#include <utils.h>

// PROGRAMA
t_config_memoria *read_config(char *config_path)
{
    t_config_memoria *memoria_config = malloc(sizeof(t_config_memoria));
    t_config *config_aux = config_create(config_path);

    if (config_aux == NULL)
    {
        log_error(logger_aux, "No se encontro el archivo de configuración");
        return NULL;
    }

    memoria_config->port = string_duplicate(config_get_string_value(config_aux, "PUERTO_ESCUCHA"));
    memoria_config->memory_size = config_get_int_value(config_aux, "TAM_MEMORIA");
    memoria_config->segment_zero_size = config_get_int_value(config_aux, "TAM_SEGMENTO_0");
    memoria_config->segment_quantity = config_get_int_value(config_aux, "CANT_SEGMENTOS");
    memoria_config->memory_time_delay = config_get_int_value(config_aux, "RETARDO_MEMORIA");
    memoria_config->compactation_time_delay = config_get_int_value(config_aux, "RETARDO_COMPACTACION");
    memoria_config->compactation_algorithm = string_duplicate(config_get_string_value(config_aux, "ALGORITMO_ASIGNACION"));

    log_info(logger_aux, "Archivo de configuración leído correctamente");

    config_destroy(config_aux);
    return memoria_config;
}

void end_program(t_log *logger_main, t_log *logger_aux, t_config_memoria *config)
{
    log_debug(logger_aux, "Finalizando programa");
    // Logs destroy
    log_destroy(logger_main);
    log_destroy(logger_aux);
    // Memoria Config destroy
    free(config->compactation_algorithm);
    free(config);
}

// FILTROS/BUSCADORES
t_segment *get_segment_by_id(int id){
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *aux_table = list_get(all_segments_tables, i);
        for (int j = 0; j < aux_table->segment_list->elements_count; j++)
        {
            t_segment* segment = list_get(aux_table->segment_list, j);
            if (segment->id == id)
            {
                return segment;
            }
        }
    }
    return NULL;
}

t_segment *get_segment_by_address(int address)
{
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *aux_table = list_get(all_segments_tables, i);
        for (int j = 0; j < aux_table->segment_list->elements_count; j++)
        {
            t_segment *segment = list_get(aux_table->segment_list, j);
            if (segment->base_address == address)
            {
                return segment;
            }
        }
    }
    return NULL;
}

t_segments_table *get_segments_table_by_pid(int pid)
{
    t_segments_table *segments_table;
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        segments_table = list_get(all_segments_tables, i);
        if (segments_table->pid == pid)
            return segments_table;
    }
    return NULL;
}

// BITARRAY
void bitarray_clean_from_and_how_many(t_bitarray *bitmap, int from, int how_many)
{
    for (int i = from; i < (from + how_many); i++)
    {
        bitarray_clean_bit(bitmap, i);
    }
}

void bitarray_clean_all(t_bitarray *bitmap)
{
    for (int i = 0; i < bitarray_get_max_bit(bitmap); i++)
    {
        bitarray_clean_bit(bitmap, i);
    }
}

// DATOS
void move_data(int to, int from, int length)
{
    memcpy(memory_space + to, memory_space + from, length);
}