#include "filesystem.h"

int main()
{
    // Inicializo los logs
    logger_main = log_create(logger_main_path, "FILESYSTEM", true, LOG_LEVEL_INFO);
    logger_aux = log_create(logger_aux_path, "FILESYSTEM AUX", true, LOG_LEVEL_DEBUG);
    // configuracion
    config = read_config(config_path, logger_main);
    if (config == NULL)
    {
        end_program(logger_main, config, logger_aux);
        return EXIT_FAILURE;
    }
    initialize_filesystem();

    // filesystem en MODO cliente de memoria
    log_info(logger_aux, "Iniciando cliente");
    // create_connection(config->ip_memoria, config->puerto_memoria, logger_aux);
    memory_socket = start_memory_client(config->ip_memoria, config->puerto_memoria, logger_aux);
    // filesystem en MODO servidor de kernel
    log_info(logger_aux, "Iniciando servidor");
    start_filesystem_server();
    send_end(memory_socket, logger_aux);
    // terminar programa
    end_program(logger_main, config, logger_aux);

    return EXIT_SUCCESS;
}
