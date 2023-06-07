#include <utils.h>

//PROGRAMA
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

//SEGMENTOS
t_segment *create_segment(int id, int size){
   if(!is_malloc_possible(size)){
      log_debug(logger_aux, "No hay espacio suficiente para crear el segmento");
      return NULL;
   }

   t_segment *segment = new_segment(id,size);

   //Esta condicion quizas deba usarla en el modulo de Kernel.
   //if(segment->base_address == -1){
   //   log_debug(logger_aux, "No se pudo crear el segmento, se necesita compactar");
   //}

   return segment;
}


t_segment *new_segment(int id, int size)
{
    t_segment *segment = malloc(sizeof(t_segment));

    segment->id = id;
    segment->size = size;

    int base_address = malloc_segment(segment);

    segment->base_address = base_address;

    if (base_address != -1)
    {
        for (int i = base_address; i < (base_address + size); i++)
        {
            bitarray_set_bit(free_space_table, i);
        }
    }

    return segment;
}

bool is_malloc_possible(int size){
   int free_space = 0;
   int fst_size = bitarray_get_max_bit(free_space_table);

   for(int i = 0; i < fst_size; i++){
      if(!bitarray_test_bit(free_space_table,i)){
         free_space++;
      }
   }

   if(free_space >= size){
      return true;
   }else{
      return false;
   }
}

int malloc_segment(t_segment *segment)
{
    int base_address;

    char *algorithm = string_duplicate(config->compactation_algorithm);

    if (strcmp(algorithm, "FIRST") == 0)
    {
        base_address = FIRST_FIT(segment);
    }
    else if (strcmp(algorithm, "BEST") == 0)
    {
        base_address = BEST_FIT(segment);
    }
    else if (strcmp(algorithm, "WORST") == 0)
    {
        base_address = WORST_FIT(segment);
    }
    else
    {
        log_debug(logger_aux, "No se reconoce el algoritmo de asignacion");
        return -1;
    }

    return base_address;
}

//TABLAS DE SEGMENTOS
t_segments_table *create_segments_table(int pid)
{
    t_segments_table *segments_table = malloc(sizeof(t_segments_table));
    segments_table->pid = pid;
    segments_table->segment_list = list_create();

    list_add(segments_table->segment_list, segment_0);

    list_add(all_segments_tables, segments_table);

    return segments_table;
}

void add_segment_to_table(t_segments_table *segments_table, t_segment *segment)
{
    list_add(segments_table->segment_list, segment);
}

void delete_segments_table(t_segments_table *segments_table)
{
    // Inicio "i" en 1, ya que en 0 esta el segmento cero
    for (int i = 1; i < list_size(segments_table->segment_list); i++)
    {
        list_remove_and_destroy_element(segments_table->segment_list, i, free);
    }
    list_destroy(segments_table->segment_list);
    free(segments_table);
}

//DATOS
t_data *create_data(char *value, int data_length)
{
    t_data *data = malloc(sizeof(t_data));
    data->value = string_duplicate(value);
    data->value_length = data_length;
    return data;
}

void move_data(int to, int from, int length)
{
    memcpy(memory_space + to, memory_space + from, length);
}

void bitarray_clean_all(t_bitarray *bitmap)
{
    for (int i = 0; i < bitarray_get_max_bit(bitmap); i++)
    {
        bitarray_clean_bit(bitmap, i);
    }
}

//COMPACTACION
void compact_memory()
{
    // Setear todos los bits a 0
    bitarray_clean_all(free_space_table);

    // REALOCAR TODO:
    // Asignar el segmento 0
    segment_0->base_address = malloc_segment(segment_0);

    // Asignar a todos los demas segmentos
    int all_segments_tables_size = all_segments_tables->elements_count;
    int aux_table_size;

    for (int i = 0; i < all_segments_tables_size; i++)
    {
        t_segments_table *aux_table = list_get(all_segments_tables, i);
        aux_table_size = aux_table->segment_list->elements_count;
        for (int j = 1; j < aux_table_size; j++)
        {
            t_segment *aux_segment = list_get(aux_table->segment_list, j);
            int old_address = malloc_segment(aux_segment);
            aux_segment->base_address = malloc_segment(aux_segment);
            move_data(aux_segment->base_address, old_address, aux_segment->size);
        }
    }
}