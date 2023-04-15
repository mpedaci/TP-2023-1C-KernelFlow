#include "kernel.h"

int main(int argc, char *argv[])
{
   // Inicializo los logs
   logger_main = log_create(logger_main_path, "KERNEL", true, LOG_LEVEL_INFO);
   logger_aux = log_create(logger_aux_path, "KERNEL AUX", true, LOG_LEVEL_DEBUG);

   // Leer configuracion
   t_config_kernel *config = NULL;
   config = read_config(config_path, logger_main);
   if (config == NULL)
   {
      end_program(logger_main, logger_aux, config);
      return EXIT_FAILURE;
   }

   // Inicializar servidor
   printf("Iniciando servidor en el puerto %s\n", config->puerto_escucha);
   // start_kernel_server(config->puerto_escucha, logger_aux);

   // Iniciar clientes
   printf("Iniciando clientes\n");
   start_memory_client(config->ip_memoria, config->puerto_memoria, logger_aux);
   start_filesystem_client(config->ip_filesystem, config->puerto_filesystem, logger_aux);
   start_cpu_client(config->ip_cpu, config->puerto_cpu, logger_aux);

   // Fin del programa
   end_program(logger_main, logger_aux, config);
   return EXIT_SUCCESS;
}
