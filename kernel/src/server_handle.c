#include "server_handle.h"

void start_kernel_server(char *listen_port, t_log *logger)
{
    int server_socket = server_start(listen_port, logger);
    // Forma unico cliente
    int client_socket;
    int i = 0;
    // i < 3 Para que se conecten 3 clientes y cortar servidor
    while (i < 2)
    {
        client_socket = client_wait(server_socket, logger);
        if (client_socket == -1)
            continue; // Significa que fallo la conexion
        log_info(logger, "Se conecto un cliente");

        if (hs_server_to_module_valid(client_socket, HSKERNEL, logger))
            process_client(client_socket, logger);
        socket_destroy(client_socket);
        i++;
    }
    // Cierro todos socket
    socket_destroy(server_socket);
}

void process_client(int socket, t_log *logger)
{
    bool exit = false;
    while (exit == false)
    {
        t_package* package = get_package(socket, logger);
        switch (package->operation_code)
        {
        case INSTRUCCIONES:
            t_persona *persona = get_instrucciones(package);
            printf("Se recibio el nombre: %s\n", persona->nombre);
            printf("Se recibio la longitud de nombre: %d\n", persona->nombre_length);
            printf("Se recibio el dni: %d\n", persona->dni);
            printf("Se recibio el edad: %d\n", persona->edad);
            printf("Se recibio el pasaporte: %d\n", persona->pasaporte);
            free(persona->nombre);
            free(persona);
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