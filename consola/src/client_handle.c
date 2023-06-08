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

void send_instruccions_and_wait_answer(t_config_console *config, t_list *lista_instrucciones, t_log *logger_console)
{
    // inicializo socket
    int kernel_socket = start_console_client(config->ip_kernel, config->puerto_kernel, logger_console);
    // Enviar lista de instrucciones a kernel
    send_instrucciones(kernel_socket, lista_instrucciones, logger_console);
    // Espero que la instruccion recibida sea END para saber que finalizo
    t_package *p = get_package(kernel_socket, logger_console);
    t_instruccion *instruccion = get_instruccion(p);
    if (instruccion->identificador == I_EXIT)
    {
        log_debug(logger_console, "Instruccion recibida: %d", instruccion->identificador);
    }
    // destruyo socket paquete e instruccion
    package_destroy(p);
    destroy_instruccion(instruccion);
    socket_destroy(kernel_socket);
}