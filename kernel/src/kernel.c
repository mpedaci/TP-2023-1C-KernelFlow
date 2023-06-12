#include "kernel.h"

int main()
{
    // Para cierre seguro
    signal(SIGINT, sigintHandler);
    // 0. Inicializar loggers
    logger_main = log_create(logger_main_path, "KERNEL", true, LOG_LEVEL_INFO);
    logger_aux = log_create(logger_aux_path, "KERNEL AUX", true, LOG_LEVEL_DEBUG);
    // 1. Cargar configuracion
    config_kernel = read_config(config_path, logger_aux);
    if (config_kernel == NULL)
    {
        log_error(logger_main, "No se pudo cargar la configuracion");
        end_program(logger_main, logger_aux, config_kernel, modules_client, queues);
        return EXIT_FAILURE;
    }
    // 2. Crear clientes
    modules_client = start_modules_client(config_kernel, logger_aux);
    /* if (modules_client == NULL)
    {
        log_error(logger_main, "No se pudo crear los clientes");
        end_program(logger_main, logger_aux, config_kernel, modules_client, queues);
        return EXIT_FAILURE;
    } */
    // 3. Crear estructuras kernel
    // 3.1. Crear colas
    queues = create_queues();
    // 3.2 Crear recursos
    cargar_recursos();
    // 4. Levantar hilo de servidor
    start_kernel_server(config_kernel->puerto_escucha);
    // 4.1. Levantar servidor - OK
    // 4.2. Recibir cliente - OK
    // 4.3. Crear hilo para cliente - OK
    // 4.4. Crear PCB - OK
    // 4.5. Solicitar Tabla de Segmentos - OK
    // 4.6. Alojar PCB en cola de ready - OK
    // 4.7. Inicializar mutex - OK
    inicializar_mutex();
    // 5. Levantar hilo de kernel
    start_kernel_core();
    // 5.1. Verificar contenido de colas - OK
    // 5.2. Aplicar algoritmo - OK
    // 5.3. Procesar CPU
    // 5.4. Actualizar estados - OK
    // 6. Mostrar info del sistema
    // Mantengo abierto el programa por ahora
    sleep(3);
    while (end_program_flag == false)
        print_menu();
    // 6. Cerrar servidor - OK
    end_kernel_server();
    end_kernel_core();
    // 7. Limpiar estructuras - OK
    end_program(logger_main, logger_aux, config_kernel, modules_client, queues);
    return EXIT_SUCCESS;
}

void sigintHandler(int signum)
{
    printf("\nIniciando fin del modulo por signal: %d\n", signum);
    end_kernel_server();
    end_kernel_core();
    end_program(logger_main, logger_aux, config_kernel, modules_client, queues);
    exit(signum);
}
