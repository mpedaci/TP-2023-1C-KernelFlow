#include "server.h"

int start_server(char *port)
{
    int socket_servidor;
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &hints, &servinfo);
    // Creamos el socket de escucha del servidor
    socket_servidor = socket(servinfo->ai_family,
                             servinfo->ai_socktype,
                             servinfo->ai_protocol);
    // Asociamos el socket a un puerto
    bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
    // Escuchamos las conexiones entrantes
    listen(socket_servidor, SOMAXCONN);
    freeaddrinfo(servinfo);
    return socket_servidor;
}

int wait_client(int socket_servidor)
{
    // Aceptamos un nuevo cliente
    int socket_cliente;
    socket_cliente = accept(socket_servidor, NULL, NULL);
    return socket_cliente;
}

int get_operation(int socket_cliente)
{
    int cod_op;
    if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

void *get_buffer(int *size, int socket_cliente)
{
    void *buffer;
    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);
    return buffer;
}

char *get_message(int socket_cliente)
{
    int size;
    char *buffer = get_buffer(&size, socket_cliente);
    return buffer;
}

t_list *get_package(int socket_cliente)
{
    int size;
    int desplazamiento = 0;
    void *buffer;
    t_list *valores = list_create();
    int tamanio;
    buffer = get_buffer(&size, socket_cliente);
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
