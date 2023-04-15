#include "client_handle.h"

void start_memory_client(char *ip, char *port, t_log *logger)
{
    int client_socket = create_connection(ip, port);
    if (client_socket == -1)
    {
        log_error(logger, "No se pudo conectar al servidor de memoria");
        return;
    }
    log_info(logger, "Conectado al servidor de memoria");
    send_message("Hola, soy un cliente", client_socket);
    log_info(logger, "Mensaje enviado");
    close(client_socket);
}

void start_filesystem_client(char *ip, char *port, t_log *logger)
{
    int client_socket = create_connection(ip, port);
    if (client_socket == -1)
    {
        log_error(logger, "No se pudo conectar al servidor de filesystem");
        return;
    }
    log_info(logger, "Conectado al servidor de filesystem");
    send_message("Hola, soy un cliente", client_socket);
    log_info(logger, "Mensaje enviado");
    close(client_socket);
}

void start_cpu_client(char *ip, char *port, t_log *logger)
{
    int client_socket = create_connection(ip, port);
    if (client_socket == -1)
    {
        log_error(logger, "No se pudo conectar al servidor de cpu");
        return;
    }
    log_info(logger, "Conectado al servidor de cpu");
    send_message("Hola, soy un cliente", client_socket);
    log_info(logger, "Mensaje enviado");
    close(client_socket);
}