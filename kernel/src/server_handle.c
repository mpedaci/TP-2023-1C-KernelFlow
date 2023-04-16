#include "server_handle.h"

void start_kernel_server(char *listen_port, t_log *logger)
{
    int server_socket = start_server(listen_port);
    // Forma unico cliente
    int client_socket;
    int i = 0;
    while (i < 3)
    {
        client_socket = wait_client(server_socket);
        if (client_socket == -1)
        {
            log_error(logger, "Error al aceptar conexion");
            continue;
        }
        log_info(logger, "Se conecto un cliente");
        if (server_pass_handshake(client_socket, logger))
            process_client(client_socket, logger);
        client_destroy(client_socket);
        i++;
    }
    // Cierro todos socket
    // client_destroy(client_socket);
    server_destroy(server_socket);
}

// GENERA SEGMENTATION FAULT AL CERRAR EL SERVIDOR
void start_kernel_server_thread(char *listen_port, t_log *logger)
{
    int server_socket = start_server(listen_port);
    // Forma multiples clientes
    int i = 0;
    while (i < 3)
    {
        pthread_t thread;
        int *client_socket = malloc(sizeof(int));
        client_socket = wait_client(server_socket);
        /* if (client_socket == -1)
        {
            log_error(logger, "Error al aceptar conexion");
            continue;
        } */
        log_info(logger, "Se conecto un cliente");
       
        pthread_create(&thread,
                       NULL,
                       (void *)atender_cliente,
                       client_socket);
        pthread_detach(thread);

        i++;
    }
    // Cierro todos socket
    // client_destroy(client_socket);
    server_destroy(server_socket);
}

void atender_cliente(int socket_cliente)
{
    /* if (server_pass_handshake(*socket_cliente, logger)) */
    t_list *lista;
    bool exit = false;
    while (exit == false)
    {
        int cod_op = get_operation(socket_cliente);
        printf("Recibido codigo de operacion %d\n", cod_op);
        switch (cod_op)
        {
        case MENSAJE:
            char *message = get_message(socket_cliente);
            printf("Recibido mensaje %s\n", message);
            free(message);
            break;
        case PAQUETE:
            lista = get_package(socket_cliente);
            list_iterate(lista, (void *)iteratorFunction);
            break;
        case -1:
            printf("el cliente se desconecto. Terminando servidor");
            exit = true;
            break;
        default:
            printf("Default %d\n", cod_op);
            printf("Operacion desconocida.");
            break;
        }
    }
    client_destroy(socket_cliente);
    free(socket_cliente);
}

void process_client(int client_socket, t_log *logger)
{
    t_list *lista;
    bool exit = false;
    while (exit == false)
    {
        int cod_op = get_operation(client_socket);
        printf("Recibido codigo de operacion %d\n", cod_op);
        switch (cod_op)
        {
        case MENSAJE:
            char *message = get_message(client_socket);
            log_debug(logger, "Recibido mensaje %s\n", message);
            free(message);
            break;
        case PAQUETE:
            lista = get_package(client_socket);
            list_iterate(lista, (void *)iteratorFunction);
            break;
        case -1:
            log_error(logger, "el cliente se desconecto. Terminando servidor");
            exit = true;
            break;
        default:
            printf("Default %d\n", cod_op);
            log_warning(logger, "Operacion desconocida.");
            break;
        }
    }
    // SEGMENTATION FAULT
    // list_destroy(lista);
}

void iteratorFunction(char *value)
{
    printf("Recibido valor %s\n", value);
}