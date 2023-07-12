#include "server.h"

int server_start(char *port, t_log *logger)
{
    int server_socket;
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &hints, &servinfo);
    // Creamos el socket de escucha del servidor
    server_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    // Set the socket to non-blocking mode
    int flags = fcntl(server_socket, F_GETFL, 0);
    if (flags == -1)
    {
        log_error(logger, "Error getting socket flags");
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(server_socket, F_SETFL, flags) == -1)
    {
        log_error(logger, "Error setting socket to non-blocking");
        return -1;
    }
    const int enable = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        log_error(logger, "setsockopt(SO_REUSEADDR) failed");
        return -1;
    }
    // Asociamos el socket a un puerto
    if (bind(server_socket, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
    {
        log_error(logger, "Error al asignar una direccion al socket");
        freeaddrinfo(servinfo);
        return -1;
    }
    // Escuchamos las conexiones entrantes
    if (listen(server_socket, SOMAXCONN) < 0)
    {
        log_error(logger, "Error al escuchar conexiones entrantes");
        freeaddrinfo(servinfo);
        return -1;
    }
    log_info(logger, "Servidor escuchando en el puerto: %s", port);
    freeaddrinfo(servinfo);
    return server_socket;
}

int client_wait(int server_socket, t_log *logger)
{
    // Aceptamos un nuevo cliente
    int client_socket;
    int timeout = 2; // En segundos
    for (int i = 0; i < timeout; i++)
    {
        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0)
        {
            sleep(1);
            continue;
        }
        else
            break;
    }
    if (client_socket < 0)
        return -1;
    return client_socket;
}
