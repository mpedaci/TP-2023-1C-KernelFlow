#include <utils.h>

// PROGRAMA
t_config_memoria *read_config(char *config_path)
{
    t_config_memoria *memoria_config = malloc(sizeof(t_config_memoria));
    t_config *config_aux = config_create(config_path);

    if (config_aux == NULL)
    {
        log_error(logger_aux, "No se encontro el archivo de configuracion");
        return NULL;
    }

    memoria_config->port = string_duplicate(config_get_string_value(config_aux, "PUERTO_ESCUCHA"));
    memoria_config->memory_size = config_get_int_value(config_aux, "TAM_MEMORIA");
    memoria_config->segment_zero_size = config_get_int_value(config_aux, "TAM_SEGMENTO_0");
    memoria_config->segment_quantity = config_get_int_value(config_aux, "CANT_SEGMENTOS");
    memoria_config->memory_time_delay = config_get_int_value(config_aux, "RETARDO_MEMORIA");
    memoria_config->compactation_time_delay = config_get_int_value(config_aux, "RETARDO_COMPACTACION");
    memoria_config->compactation_algorithm = string_duplicate(config_get_string_value(config_aux, "ALGORITMO_ASIGNACION"));

    log_info(logger_aux, "Archivo de configuracion leido correctamente");

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
    free(config->port);
    free(config->compactation_algorithm);
    free(config);
}

// FILTROS/BUSCADORES
t_segment *get_segment_by_id(int id)
{
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *aux_table = list_get(all_segments_tables, i);
        for (int j = 0; j < aux_table->segment_list->elements_count; j++)
        {
            t_segment *segment = list_get(aux_table->segment_list, j);
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
        bitarray_set(bitmap, i, false);
    }
}

void bitarray_clean_all(t_bitarray *bitmap)
{
    for (int i = 0; i < bitmap->size; i++)
    {
        bitarray_set(bitmap, i, false);
    }
}

int get_free_space(int base)
{
    int size = 0;
    for (int i = base; i < free_space_table->size; i++)
    {
        if (!bitarray_get(free_space_table, i))
            size++;
        else
            break;
    }
    return size;
}
// DATOS
t_info *read_memory(int base_address, int size)
{
    sleep(config->memory_time_delay / 1000);

    t_info *info = malloc(sizeof(t_info));
    info->data = malloc(size);
    info->size = size;

    if (base_address + size > config->memory_size)
    {
        log_error(logger_aux, "Se intento leer fuera de la memoria");
        return NULL;
    }
    else
    {
        pthread_mutex_lock(&memory_space_mutex);
        memcpy(info->data, memory_space + base_address, size);
        pthread_mutex_unlock(&memory_space_mutex);
        return info;
    }
}

bool write_memory(int base_address, int size, void *data)
{
    sleep(config->memory_time_delay / 1000);

    if (base_address + size > config->memory_size)
    {
        log_error(logger_aux, "Se intento escribir fuera de la memoria");
        return false;
    }
    else
    {
        pthread_mutex_lock(&memory_space_mutex);
        memcpy(memory_space + base_address, data, size);
        pthread_mutex_unlock(&memory_space_mutex);
        return true;
    }
}

void move_data(int to, int from, int length)
{
    pthread_mutex_lock(&memory_space_mutex);
    memcpy(memory_space + to, memory_space + from, length);
    pthread_mutex_unlock(&memory_space_mutex);
}

// IMPRESION DE SEGMENTOS
void print_all_segments_tables()
{
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *aux_table = (t_segments_table *)list_get(all_segments_tables, i);
        for (int j = 1; j < list_size(aux_table->segment_list); j++)
        {
            t_segment *segment = (t_segment *)list_get(aux_table->segment_list, j);
            log_info(logger_main, "PID: %d - Segmento: %d - Base: %d - Tamanio: %d", aux_table->pid, segment->id, segment->base_address, segment->size);
        }
    }
}

int get_pid_by_address(int address)
{
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *aux_table = (t_segments_table *)list_get(all_segments_tables, i);
        for (int j = 0; j < list_size(aux_table->segment_list); j++)
        {
            t_segment *segment = (t_segment *)list_get(aux_table->segment_list, j);
            if (address >= segment->base_address && address < (segment->base_address + segment->size))
                return aux_table->pid;
        }
    }
    return -1;
}