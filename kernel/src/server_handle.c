#include "server_handle.h"

// Multihilo - Paralelo

t_client_connection *connection;

void start_kernel_server(char *listen_port)
{
    server_socket = server_start(listen_port, logger_aux);
    while (accept_connections)
    {
        connection = (t_client_connection *)malloc(sizeof(t_client_connection));
        connection->pid = pid_counter;
        connection->socket = client_wait(server_socket, logger_aux);
        if (connection->socket != -1)
        {
            pid_counter++;
            pthread_create(&thr_server, 0, process_client_entry, (void *)connection);
        }else {
            free(connection);
        }
    }
}

void end_kernel_server()
{
    accept_connections = false;
    pthread_join(thr_server, NULL); 
    socket_destroy(server_socket);
    free(connection);
}

void *process_client_entry(void *ptr)
{
    if (!ptr)
        pthread_exit(0);
    t_client_connection *conn = (t_client_connection *)ptr;
    log_info(logger_aux, "Thread con PID: %d iniciado", conn->pid);
    if (hs_server_to_module_valid(conn->socket, HSKERNEL, logger_aux))
        process_client_communication(conn);
    socket_destroy(conn->socket);
    free(conn);
    pthread_exit(0);
}

// Comunicacion con el cliente

void process_client_communication(t_client_connection *conn)
{
    bool exit = false;
    while (exit == false)
    {
        t_package *package = get_package(conn->socket, logger_aux);
        switch (package->operation_code)
        {
        case INSTRUCCIONES:
            // t_instruccion **instrucciones = get_instrucciones(package);
            // t_pcb *pcb = pcb_create(conn->pid, instrucciones);
            // send_instruccion(modules_client->memory_client_socket, "TABLA SEGMENTOS NUEVA", logger_aux);
            // t_package *package = get_package(modules_client->memory_client_socket, logger_aux);
            // t_tabla_segmentos *tabla_segmentos = get_tabla_segmentos(package);
            // pcb->segments_table = tabla_segmentos;
            // list_add(queues->NEW, pcb);

            t_persona *persona = get_instrucciones(package);
            printf("Thread con PID: %d\n", conn->pid);
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

