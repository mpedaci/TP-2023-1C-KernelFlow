#include "server_handle.h"

void start_cpu_server(char *listen_port, t_log *logger) {
    int server_socket = start_server(listen_port);

    int client_socket;

    client_socket = wait_client(server_socket);
    if (client_socket == -1)
    {
        log_error(logger, "Error al aceptar conexion");
        return;
    }
    log_info(logger, "Se conecto un cliente");
    if (server_pass_handshake(client_socket, logger))
        process_client(client_socket, logger);
    client_destroy(client_socket);
    
    // Cierro todos socket
    client_destroy(client_socket);
    server_destroy(server_socket);
}

void process_client(int client_socket, t_log *logger) {
    t_list *lista;
    while (1)
    {
        int cod_op = get_operation(client_socket);
        printf("Recibido codigo de operacion %d\n", cod_op);
        switch (cod_op)
        {
        case MENSAJE:
            char* message = get_message(client_socket);
            printf("Recibi el mensaje: %s", message);
            free(message);
            break;
        case PAQUETE:
            lista = get_package(client_socket);
            list_iterate(lista, (void *)iteratorFunction);
            break;
        case -1:
            log_error(logger, "el cliente se desconecto. Terminando servidor");
            break;
        default:
            printf("Default %d\n", cod_op);
            log_warning(logger, "Operacion desconocida.");
            break;
        }
    }
}

void iteratorFunction(char *value)
{
    printf("Recibido valor %s\n", value);
}


void send_context_kernel(t_execution_context* context) {
    // enviar mensaje al kernel con el contexto de ejecucion, hablar con equipo para ver como mandamos esto #########################################
}