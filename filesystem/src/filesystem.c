#include "filesystem.h"

int main()
{
    // Inicializo los logs
    logger_main = log_create(logger_main_path, "FILESYSTEM", true, LOG_LEVEL_DEBUG);
    logger_aux = log_create(logger_aux_path, "FILESYSTEM AUX", true, LOG_LEVEL_DEBUG);
    // configuracion
    config_fs = read_config(config_path, logger_main);
    if (config_fs == NULL)
    {
        end_program(logger_main, config_fs, logger_aux);
        return EXIT_FAILURE;
    }
    log_debug(logger_main, "Configuracion cargada");
    if (!initialize_filesystem())
    {
        end_filesystem();
        end_program(logger_main, config_fs, logger_aux);
        return EXIT_FAILURE;
    }
    log_debug(logger_main, "Filesystem inicializado");
    // filesystem en MODO cliente de memoria
    log_info(logger_aux, "Iniciando cliente");
    memory_socket = start_memory_client(config_fs->ip_memoria, config_fs->puerto_memoria, logger_aux);
    // filesystem en MODO servidor de kernel
    log_info(logger_aux, "Iniciando servidor");
    start_filesystem_server();
    send_end(memory_socket, logger_aux);
    // terminar programa
    pthread_join(thr_requestAsync, NULL);
    end_filesystem();
    end_program(logger_main, config_fs, logger_aux);

    return EXIT_SUCCESS;
}
