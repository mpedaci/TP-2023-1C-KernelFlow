#include "filesystem_server.h"

void start_filesystem_server(char *listen_port, t_log *logger)
{
    int server_socket = server_start(listen_port, logger);
    int client_socket;
    client_socket = client_wait(server_socket, logger);
    log_info(logger, "Se conecto un cliente");

    if (hs_server_to_module_valid(client_socket, HSFS, logger))
        process_client(client_socket, logger);

    log_info(logger, "Cerrando conexion con cliente");
    socket_destroy(client_socket);
    socket_destroy(server_socket);
}

void process_client(int client_socket, t_log *logger) {
    bool exit = false;
    while (exit == false)
    {
        t_package* package = get_package(client_socket, logger);
        switch (package->operation_code)
        {
        case OFILE:
            printf("RECIBIDO INST OFILE");
            package_destroy(package);
            break;
        case END:
            printf("Conexion Finalizada");
            exit = true;
            break;
        default:
            printf("Operacion desconocida.");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}