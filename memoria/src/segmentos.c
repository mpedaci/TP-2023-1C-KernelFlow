#include "segmentos.h"

// SEGMENTOS

t_segment *create_segment(int id, int size)
{
    if (!is_malloc_possible(size))
    {
        log_error(logger_aux, "No hay espacio suficiente para crear el segmento");
        return NULL;
    }
    t_segment *segment = new_segment(id, size);
    return segment;
}

t_segment *new_segment(int id, int size)
{
    t_segment *segment = malloc(sizeof(t_segment));
    segment->id = id;
    segment->size = size;
    segment->base_address = get_base_adress(segment);
    return segment;
}

/*
    Cuando eliminas un segmento, el memory_space no sufre cambios, solo la tabla de huecos libres.
    Esta funcion, permite que se vuelva poder a escribir en el espacio que ocupaba el segmento eliminado.
*/
void delete_segment(int pid, t_segment *segment)
{
    t_segments_table *segments_table = get_segments_table_by_pid(pid);
    int base_address = segment->base_address;
    list_remove_element(segments_table->segment_list, segment);
    bitarray_clean_from_and_how_many(free_space_table, base_address, segment->size);
    free(segment);
}

bool is_malloc_possible(int size)
{
    int free_space = 0;
    for (int i = 0; i < free_space_table->size; i++)
    {
        if (!bitarray_get(free_space_table, i))
        {
            free_space++;
        }
    }
    return free_space >= size;
}

int get_base_adress(t_segment *segment)
{
    int base_address = -1;
    char *algorithm = string_duplicate(config->compactation_algorithm);
    if (strcmp(algorithm, "FIRST") == 0)
        base_address = FIRST_FIT(segment);
    else if (strcmp(algorithm, "BEST") == 0)
        base_address = BEST_FIT(segment);
    else if (strcmp(algorithm, "WORST") == 0)
        base_address = WORST_FIT(segment);
    else
        log_warning(logger_aux, "No se reconoce el algoritmo de asignacion");
    if (base_address != -1)
        for (int i = base_address; i < (base_address + segment->size); i++)
            bitarray_set(free_space_table, i, true);
    free(algorithm);
    return base_address;
}

// TABLAS DE SEGMENTOS

t_segments_table *create_segments_table(int pid)
{
    t_segments_table *segments_table = NULL;
    segments_table = get_segments_table_by_pid(pid);
    if (segments_table == NULL){
        segments_table = malloc(sizeof(t_segments_table));
        segments_table->pid = pid;
        segments_table->segment_list = list_create();
        list_add(segments_table->segment_list, segment_0);
        list_add(all_segments_tables, segments_table);
    }
    return segments_table;
}

void add_segment_to_table(int pid, t_segment *segment)
{
    t_segments_table *segments_table = get_segments_table_by_pid(pid);
    if (segments_table == NULL)
        segments_table = create_segments_table(pid);
    list_add(segments_table->segment_list, segment);
}

void delete_segments_table(t_segments_table *segments_table)
{
    list_remove_element(all_segments_tables, segments_table);
    for (int i = 1; i < list_size(segments_table->segment_list); i++){
        t_segment *segment = list_get(segments_table->segment_list, i);
        bitarray_clean_from_and_how_many(free_space_table, segment->base_address, segment->size);
        free(segment);
    }
    list_destroy(segments_table->segment_list);
    free(segments_table);
}