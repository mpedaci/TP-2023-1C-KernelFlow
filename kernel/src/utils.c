#include <utils.h>

t_config_kernel *read_config(char *config_path, t_log *logger)
{
   t_config_kernel *kernel_config = malloc(sizeof(t_config_kernel));
   t_config *config = config_create(config_path);

   if (config == NULL)
   {
      log_error(logger, "No se pudo leer el archivo de configuración");
      return NULL;
   }

   kernel_config->ip_memoria = string_duplicate(config_get_string_value(config, "IP_MEMORIA"));
   kernel_config->puerto_memoria = string_duplicate(config_get_string_value(config, "PUERTO_MEMORIA"));
   kernel_config->ip_filesystem = string_duplicate(config_get_string_value(config, "IP_FILESYSTEM"));
   kernel_config->puerto_filesystem = string_duplicate(config_get_string_value(config, "PUERTO_FILESYSTEM"));
   kernel_config->ip_cpu = string_duplicate(config_get_string_value(config, "IP_CPU"));
   kernel_config->puerto_cpu = string_duplicate(config_get_string_value(config, "PUERTO_CPU"));
   kernel_config->puerto_escucha = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA"));

   log_debug(logger, "Archivo de configuración leído correctamente");

   config_destroy(config);
   return kernel_config;
}

void end_program(t_log *logger_main, t_log *logger_aux, t_config_kernel *config)
{
   log_debug(logger_aux, "Finalizando programa");
   // Logs destroy
   log_destroy(logger_main);
   log_destroy(logger_aux);
   // Kernel Config destroy
   free(config->ip_cpu);
   free(config->ip_filesystem);
   free(config->ip_memoria);
   free(config->puerto_cpu);  
   free(config->puerto_escucha);
   free(config->puerto_filesystem);
   free(config->puerto_memoria); 
   free(config);
}