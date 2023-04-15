#include "server_handle.h"

void start_kernel_server(char *listen_port, t_log *logger)
{
    int server_socket = start_server(listen_port);
    // Forma unico cliente
    int client_socket = wait_client(server_socket);
    t_list *lista;
    bool exit = false;
    while (exit == false)
    {
        int cod_op = get_operation(client_socket);
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
        default:
            log_warning(logger, "Operacion desconocida.");
            break;
        }
    }
    // Cierro todos socket
    client_destroy(client_socket);
    server_destroy(server_socket);
}

void iteratorFunction(char *value)
{
    printf("Recibido valor %s\n", value);
}