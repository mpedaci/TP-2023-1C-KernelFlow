#include "client_handle.h"

int start_console_client(char *ip_kernel, char *puerto_kernel, t_log *logger_console)
{
    int kernel_socket = create_connection(ip_kernel, puerto_kernel, logger_console);
    if (kernel_socket == -1)
    {
        log_error(logger_console, "No se pudo conectar al servidor Kernel");
        return -1;
    }
    if (!hs_client_to_module_valid(kernel_socket, HSCONSOLA, HSKERNEL, logger_console))
        return -1;
    log_debug(logger_console, "Conexion exitosa con kernel");
    return kernel_socket;
}

void send_instruccions_and_wait_to_end(t_config_console *config, t_list *lista_instrucciones, t_log *logger_console)
{
    // inicializo socket
    int kernel_socket = start_console_client(config->ip_kernel, config->puerto_kernel, logger_console);
    if (kernel_socket != -1)
    {
        // Enviar lista de instrucciones a kernel
        send_instrucciones(kernel_socket, lista_instrucciones, logger_console);
        // Espero confirmacion de creacion de Proceso
        wait_pid(kernel_socket, logger_console);
        // Espero que la instruccion recibida sea END para saber que finalizo
        wait_end(kernel_socket, logger_console);
        socket_destroy(kernel_socket);
    }
}

void wait_pid(int kernel_socket, t_log *logger_console)
{
    t_package *p = get_package(kernel_socket, logger_console);
    switch (p->operation_code)
    {
    case PID_STATUS:
        t_pid_status *pid_status = get_pid_status(p);
        log_debug(logger_console, "Proceso iniciado - PID: %d", pid_status->pid);
        free(pid_status);
        break;
    default:
        log_warning(logger_console, "Operacion desconocida - Finalizando");
        break;
    }
    package_destroy(p);
}

void wait_end(int kernel_socket, t_log *logger_console)
{
    t_package *p = get_package(kernel_socket, logger_console);
    switch (p->operation_code)
    {
    case PID_STATUS:
        t_pid_status *pid_status = get_pid_status(p);
        log_debug(logger_console, "Proceso finalizado: PID - %d | Status - %s", pid_status->pid, status_code_string(pid_status->status));
        free(pid_status);
        break;
    default:
        log_warning(logger_console, "Operacion desconocida - Finalizando");
        break;
    }
    package_destroy(p);
}