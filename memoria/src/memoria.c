#include "memoria.h"

int main(void)
{
    // Para cierre seguro
    signal(SIGINT, sigintHandler);
    // INICIALIZAR LOGS
    logger_main = log_create(logger_main_path, "Memoria", 1, LOG_LEVEL_DEBUG);
    logger_aux = log_create(logger_aux_path, "Memoria Aux", 1, LOG_LEVEL_DEBUG);
    // INICIALIZAR MEMORIA
    config = read_config(config_path);
    if (config == NULL)
    {
        end_program(logger_main, logger_aux, config);
        return EXIT_FAILURE;
    }
    start_memory(config);
    // INICIALIZO SERVIDOR
    start_memory_server(config->port);

    sleep(3);
    while (end_program_flag == false)
        print_menu();

    end_memory_server();
    end_memory();
    end_program(logger_main, logger_aux, config);

    return EXIT_SUCCESS;
};

void sigintHandler(int signum)
{
    printf("\nIniciando fin del modulo por signal: %d\n", signum);
    end_memory_server();
    end_memory();
    end_program(logger_main, logger_aux, config);
    exit(signum);
}