#include "server_handle.h"

int pid_counter = 0;

// Un hilo - Serie

void start_kernel_server_one_thread(char *listen_port, t_log *logger)
{
    int server_socket = server_start(listen_port, logger);
    // Forma unico cliente
    int client_socket;
    while (pid_counter < 2)
    {
        client_socket = client_wait(server_socket, logger);
        if (client_socket == -1)
            continue; // Significa que fallo la conexion
        log_info(logger, "Se conecto un cliente");

        if (hs_server_to_module_valid(client_socket, HSKERNEL, logger))
            process_client_communication(client_socket, 1, logger);
        socket_destroy(client_socket);
        pid_counter++;
    }
    // Cierro todos socket
    socket_destroy(server_socket);
}

// Multihilo - Paralelo

void start_kernel_server(char *listen_port, t_log *logger)
{
    t_client_connection *connection;
    pthread_t thread;
    int server_socket = server_start(listen_port, logger);
    while (1)
    {
        pid_counter++;
        connection = (t_client_connection *)malloc(sizeof(t_client_connection));
        connection->logger = logger;
        connection->pid = pid_counter;
        connection->sock = client_wait(server_socket, logger);
        if (connection->sock == -1)
            free(connection);
        else
        {
            pthread_create(&thread, 0, process_client_entry, (void *)connection);
            // pthread_detach(thread); // Esto permite que el thread se libere cuando termina pero no espera en cierre del programa
        }
    }
    pthread_join(thread, NULL); // Espera en cierra del programa
    socket_destroy(server_socket);
}

void *process_client_entry(void *ptr)
{
    t_client_connection *conn;
    if (!ptr)
        pthread_exit(0);
    conn = (t_client_connection *)ptr;
    printf("Thread con PID: %d iniciado\n", conn->pid);
    if (hs_server_to_module_valid(conn->sock, HSKERNEL, conn->logger))
        process_client_communication(conn->sock, conn->pid, conn->logger);
    /* close socket and clean up */
    close(conn->sock);
    free(conn);
    pthread_exit(0);
}

// Comunicacion con el cliente

void process_client_communication(int socket, int pid, t_log *logger)
{
    bool exit = false;
    while (exit == false)
    {
        t_package *package = get_package(socket, logger);
        switch (package->operation_code)
        {
        case INSTRUCCIONES:
            t_persona *persona = get_instrucciones(package);
            printf("Thread con PID: %d\n", pid);
            printf("Se recibio el nombre: %s\n", persona->nombre);
            printf("Se recibio la longitud de nombre: %d\n", persona->nombre_length);
            printf("Se recibio el dni: %d\n", persona->dni);
            printf("Se recibio el edad: %d\n", persona->edad);
            printf("Se recibio el pasaporte: %d\n", persona->pasaporte);
            free(persona->nombre);
            free(persona);
            break;
        case END:
            printf("Conexion Finalizada\n");
            exit = true;
            break;
        default:
            printf("Operacion desconocida\n");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}