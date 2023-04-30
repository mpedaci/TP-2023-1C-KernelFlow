#include "consola.h"

int main(int argc, char** argv){

  // inicializo loggers
  logger_aux = log_create(logger_aux_path, "Consola_aux", 1, LOG_LEVEL_DEBUG);
  logger_console = log_create(logger_console_path, "Consola", 1, LOG_LEVEL_DEBUG);

  // leer configuracion
  t_config_console* config = NULL;
  config = read_config(config_console_path, logger_console);

  if(config == NULL){
      end_program(logger_console, logger_aux, config);
      return EXIT_FAILURE;
    }

  if(logger_console == NULL){
      log_error(logger_console, "No se pudo iniciar la consola"),
      end_program(logger_console, logger_aux, config);
      return EXIT_FAILURE;
   }

  
  log_debug(logger_console, "Consola iniciada.");

  //inicializo socket
  start_console_client(config->ip_kernel, config->puerto_kernel, logger_console);

  // fin del programa
  end_program(logger_console, logger_aux, config);
  return EXIT_SUCCESS;
}




