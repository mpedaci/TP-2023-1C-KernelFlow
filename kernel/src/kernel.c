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
      end_program(logger_main, logger_aux, config, NULL);
      return EXIT_FAILURE;
   }

   // Iniciar clientes
   log_info(logger_aux, "Iniciando clientes");
   t_modules_client *modules_client = start_modules_client(config, logger_aux);

   if (modules_client == NULL)
   {
      end_program(logger_main, logger_aux, config, modules_client);
      return EXIT_FAILURE;
   }

   // Inicializar servidor
   log_info(logger_aux, "Iniciando servidor");
   start_kernel_server_one_thread(config->puerto_escucha, logger_aux);
   // start_kernel_server(config->puerto_escucha, logger_aux); // Multihilo

   // Fin del programa
   end_program(logger_main, logger_aux, config, modules_client);
   return EXIT_SUCCESS;
}
