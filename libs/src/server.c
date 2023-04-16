#include "server.h"

int start_server(char *port)
{
    int server_socket;
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &hints, &servinfo);
    // Creamos el socket de escucha del servidor
    server_socket = socket(servinfo->ai_family,
                             servinfo->ai_socktype,
                             servinfo->ai_protocol);
    // Asociamos el socket a un puerto
    bind(server_socket, servinfo->ai_addr, servinfo->ai_addrlen);
    // Escuchamos las conexiones entrantes
    listen(server_socket, SOMAXCONN);
    freeaddrinfo(servinfo);
    return server_socket;
}

int wait_client(int server_socket)
{
    // Aceptamos un nuevo cliente
    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);
    return client_socket;
}

bool server_pass_handshake(int client_socket, t_log *logger)
{
    log_info(logger, "Realizando handshake");
    int handshake;
    recv(client_socket, &handshake, sizeof(int), MSG_WAITALL);
    int result = handshake == HSHELO ? HSOK : HSFAIL;
    send(client_socket, &result, sizeof(int), 0);
    if (result == -1)
    {
        log_error(logger, "Handshake fallido");
        return false;
    }else {
        log_info(logger, "Handshake exitoso");
        return true;
    }
}

int get_operation(int client_socket)
{
    int cod_op;
    if (recv(client_socket, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        return cod_op;
    else
    {
        close(client_socket);
        return -1;
    }
}

void *get_buffer(int *size, int client_socket)
{
    void *buffer;
    recv(client_socket, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(client_socket, buffer, *size, MSG_WAITALL);
    return buffer;
}

char *get_message(int client_socket)
{
    int size;
    char *buffer = get_buffer(&size, client_socket);
    return buffer;
}

t_list *get_package(int client_socket)
{
    int size;
    int desplazamiento = 0;
    void *buffer;
    t_list *valores = list_create();
    int tamanio;
    buffer = get_buffer(&size, client_socket);
    while (desplazamiento < size)
    {
        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        char *valor = malloc(tamanio);
        memcpy(valor, buffer + desplazamiento, tamanio);
        desplazamiento += tamanio;
        list_add(valores, valor);
    }
    free(buffer);
    return valores;
}

void server_destroy(int socket_server)
{
    close(socket_server);
}

void client_destroy(int socket_client)
{
    close(socket_client);
}