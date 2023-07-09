#include "consola.h"

int main(int argc, char **argv)
{
    // inicializo loggers
    logger_console = log_create(logger_console_path, "Consola", 1, LOG_LEVEL_DEBUG);

    // leer configuracion
    // char *path_console_config = "./config/console.config";
    char *path_console_config = argv[1];
    t_config_console *config = NULL;
    config = read_config(path_console_config, logger_console);

    // abro archivo de instrucciones
    // char *path_pseudocodigo = "./pseudocodigo.txt";
    char *path_pseudocodigo = argv[2];
    FILE *file_instructions = fopen(path_pseudocodigo, "rt");

    if (config == NULL || logger_console == NULL)
    {
        log_error(logger_console, "No se pudo iniciar la consola");
        end_program(logger_console, config, file_instructions);
        return EXIT_FAILURE;
    }

    if (file_instructions == NULL)
    {
        log_error(logger_console, "No se pudo abrir el archivo de instrucciones");
        end_program(logger_console, config, file_instructions);
        return EXIT_FAILURE;
    }

    log_debug(logger_console, "Consola iniciada");

    // parsear archivo pseudocodigo
    t_list *lista_instrucciones = parsear_pseudocodigo(file_instructions, logger_console);

    if (lista_instrucciones == NULL)
    {
        end_program(logger_console, config, file_instructions);
        return 0;
    }

    send_instruccions_and_wait_to_end(config, lista_instrucciones, logger_console);

    // fin del programa
    end_program(logger_console, config, file_instructions);
    lista_instrucciones_destroy(lista_instrucciones);

    return EXIT_SUCCESS;
}
