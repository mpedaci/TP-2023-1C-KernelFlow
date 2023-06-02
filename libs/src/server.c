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
    // Asociamos el socket a un puerto
    if (bind(server_socket, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        log_error(logger, "Error al asignar una direccion al socket");
        freeaddrinfo(servinfo);
        return -1;
    }
    // Escuchamos las conexiones entrantes
    if (listen(server_socket, SOMAXCONN) < 0) {
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
    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
        log_error(logger, "Error al aceptar un nuevo cliente");
        return -1;
    }
    return client_socket;
}