#include "client.h"

int create_connection(char *ip, char *puerto, t_log *logger)
{
    struct addrinfo hints;
    struct addrinfo *server_info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(ip, puerto, &hints, &server_info);
    // Ahora vamos a crear el socket.
    int socket_cliente = 0;
    socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    // Ahora que tenemos el socket, vamos a conectarlo
    int res = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
    if (res == -1) {
        log_error(logger, "Error al conectar el socket");
    }
    freeaddrinfo(server_info);
    return (res == -1) ? -1 : socket_cliente;
}