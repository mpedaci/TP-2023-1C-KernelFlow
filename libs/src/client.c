#include "client.h"

void *serialize_package(t_paquete *paquete, int bytes)
{
    void *magic = malloc(bytes);
    int desplazamiento = 0;
    memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
    desplazamiento += paquete->buffer->size;
    return magic;
}

int create_connection(char *ip, char *puerto)
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
    socket_cliente = socket(server_info->ai_family,
                            server_info->ai_socktype,
                            server_info->ai_protocol);
    // Ahora que tenemos el socket, vamos a conectarlo
    int res = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
    freeaddrinfo(server_info);
    if (res == -1)
        return -1;
    return socket_cliente;
}

bool client_pass_handshake(int socket_cliente, t_log *logger)
{
    log_info(logger, "Realizando handshake");
    int cod_op = HSHELO;
    int result;
    send(socket_cliente, &cod_op, sizeof(int), 0);
    recv(socket_cliente, &result, sizeof(int), MSG_WAITALL);
    if (result == -1)
    {
        log_error(logger, "Handshake fallido");
        return false;
    }else {
        log_info(logger, "Handshake exitoso");
        return true;
    }
}

void send_message(char *mensaje, int socket_cliente)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = MENSAJE;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

    // 2 * sizeof(int) porque el paquete tiene un codigo de operacion y un tamanio
    int bytes = paquete->buffer->size + 2 * sizeof(int);

    void *a_enviar = serialize_package(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    destroy_package(paquete);
}

void crear_buffer(t_paquete *paquete)
{
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;
}

t_paquete *create_package(void)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = PAQUETE;
    crear_buffer(paquete);
    return paquete;
}

void add_to_package(t_paquete *paquete, void *valor, int tamanio)
{
    // Use of uninitialised value of size 8
    // Conditional jump or move depends on uninitialised value(s)
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

    memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
    memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

    paquete->buffer->size += tamanio + sizeof(int);
}

void send_package(t_paquete *paquete, int socket_cliente)
{
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
}

void destroy_package(t_paquete *paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

void destroy_connection(int socket_cliente)
{
    close(socket_cliente);
}