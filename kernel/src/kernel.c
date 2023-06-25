#include "kernel.h"

int main()
{
    // Para cierre seguro
    signal(SIGINT, sigintHandler);
    // 0. Inicializar loggers - OK
    logger_main = log_create(logger_main_path, "KERNEL", true, LOG_LEVEL_INFO);
    logger_aux = log_create(logger_aux_path, "KERNEL AUX", true, LOG_LEVEL_DEBUG);
    // 1. Cargar configuracion - OK
    config_kernel = read_config(config_path, logger_aux);
    if (config_kernel == NULL)
    {
        log_error(logger_main, "No se pudo cargar la configuracion");
        end_program(logger_main, logger_aux, config_kernel, modules_client);
        return EXIT_FAILURE;
    }
    // 2. Crear clientes - OK
    modules_client = start_modules_client(config_kernel, logger_aux);
    if (modules_client == NULL)
    {
        log_error(logger_main, "No se pudo crear los clientes");
        end_program(logger_main, logger_aux, config_kernel, modules_client);
        return EXIT_FAILURE;
    }
    // 4. Levantar hilo de servidor - OK
    start_kernel_server(config_kernel->puerto_escucha);
    // 5. Levantar hilo de kernel - OK
    start_kernel_core();
    // 6. Mostrar info del sistema - OK
    sleep(3);
    while (end_program_flag == false)
       print_menu();
    // 7. Cerrar servidores - OK
    end_kernel_server();
    end_kernel_core();
    // 8. Limpiar estructuras - OK
    end_program(logger_main, logger_aux, config_kernel, modules_client);
    return EXIT_SUCCESS;
}

void sigintHandler(int signum)
{
    log_warning(logger_aux, "Iniciando fin del modulo por signal de apagado");
    end_kernel_server();
    end_kernel_core();
    end_program(logger_main, logger_aux, config_kernel, modules_client);
    exit(signum);
}
