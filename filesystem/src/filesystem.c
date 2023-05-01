#include "filesystem.h"
#include "archivo.h"

int main() {
   
    // Inicializo los logs
   logger_main = log_create(logger_main_path, "FILESYSTEM", true, LOG_LEVEL_INFO);
   logger_aux = log_create(logger_aux_path, "FILESYSTEM AUX", true, LOG_LEVEL_DEBUG);

    // configuracion
   t_config_filesystem *config = NULL;
   config = read_config(config_path, logger_main);
   if (config == NULL)
   {
      end_program(logger_main, logger_aux, config);
      return EXIT_FAILURE;
   }


    initialize_filesystem(config);


    // filesystem en MODO cliente de memoria
   log_info(logger_aux, "Iniciando cliente");
   create_connection(config->ip_memoria,config->puerto_memoria,logger_aux);
   //start_memory_client(config->ip_memoria,config->puerto_memoria,logger_aux);


    //filesystem en MODO servidor de kernel 
    log_info(logger_aux, "Iniciando servidor");
    server_start(config->puerto_escucha,logger_aux);
    //start_filesystem_server(config->puerto_escucha,logger_aux);


    //terminar programa 
    end_program(logger_main,logger_aux,config);

    return EXIT_SUCCESS;
}

