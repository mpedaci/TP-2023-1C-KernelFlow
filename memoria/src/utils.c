#include <utils.h>

t_config_memoria *read_config(char *config_path)
{
   t_config_memoria *memoria_config = malloc(sizeof(t_config_memoria));
   t_config *config = config_create(config_path);

   if (config == NULL)
   {
      log_error(logger_aux, "No se encontro el archivo de configuración");
      return NULL;
   }

   memoria_config->port = config_get_int_value(config,"PUERTO_ESCUCHA");
   memoria_config->memory_size = config_get_int_value(config,"TAM_MEMORIA");
   memoria_config->segment_zero_size = config_get_int_value(config,"TAM_SEGMENTO_0");
   memoria_config->segment_quantity = config_get_int_value(config,"CANT_SEGMENTOS");
   memoria_config->memory_time_delay = config_get_int_value(config,"RETARDO_MEMORIA");
   memoria_config->compactation_time_delay = config_get_int_value(config,"RETARDO_COMPACTACION");
   memoria_config->compactation_algorithm = string_duplicate(config_get_string_value(config,"ALGORITMO_ASIGNACION"));

   log_info(logger_aux, "Archivo de configuración leído correctamente");

   config_destroy(config);
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
