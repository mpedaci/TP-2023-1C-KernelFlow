#include "cpu.h"



int main() {
   
   // inicializo loggers
   logger = log_create(logger_path, "CPU", 1, LOG_LEVEL_INFO);
   logger_aux = log_create(logger_aux_path, "CPU_AUX", 1, LOG_LEVEL_DEBUG);

   // inicializo registros
   t_registers* registers = init_registers();

   // leer configuracion
   t_config_cpu* config = NULL;
   config = read_config(config_path, logger);
   if(config == NULL) {
      close_program_cpu(config, registers, logger, logger_aux);
      return EXIT_FAILURE;
   }
   

   // inicializar servidor para kernel
   log_info(logger_aux, "Iniciando servidor");
   start_cpu_server(config->puerto_escucha, logger_aux);

   // inicidando cliente memoria
   // log_info(logger_aux, "Iniciando Clientes");


   // fin del programa
   close_program_cpu(config, registers, logger, logger_aux);
   return EXIT_SUCCESS;
}