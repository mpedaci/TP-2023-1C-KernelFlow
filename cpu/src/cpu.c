#include "cpu.h"


int main() {
   
   // inicializo loggers
   logger = log_create(logger_path, "CPU", 1, LOG_LEVEL_INFO);
   logger_aux = log_create(logger_aux_path, "CPU_AUX", 1, LOG_LEVEL_DEBUG);

   // leer configuracion
   config = read_config(config_path, logger);
   if(config == NULL) {
      close_program_cpu(config, cpu_registers, logger, logger_aux);
      return EXIT_FAILURE;
   }

   cpu_registers = init_registers();
   
   // inicializar servidor para kernel
   log_info(logger_aux, "Iniciando servidor");
   start_cpu_server(config->puerto_escucha, logger_aux);

   // fin del programa
   close_program_cpu(config, cpu_registers, logger, logger_aux);
   return EXIT_SUCCESS;
}