#include "server_handle.h"

// Multihilo - Paralelo

t_client_connection *connection;

void start_kernel_server(char *listen_port)
{
    pthread_create(&thr_server, 0, start_server_listen, (void *)listen_port);
}

void *start_server_listen(void *listen_port)
{
    char *listen_port_aux = (char *)listen_port;
    server_socket = server_start(listen_port_aux, logger_aux);
    if (server_socket == -1)
    {
        accept_connections = false;
        pthread_exit(0);
    }
    while (accept_connections)
    {
        connection = (t_client_connection *)malloc(sizeof(t_client_connection));
        connection->pid = pid_counter;
        connection->socket = client_wait(server_socket, logger_aux);
        if (connection->socket != -1 && hs_server_to_module_valid(connection->socket, HSKERNEL, logger_aux))
        {
            pid_counter++;
            pthread_create(&thr_server_conn, 0, process_client_entry, (void *)connection);
        }
        else
        {
            free(connection);
        }
    }
    pthread_exit(0);
}

void end_kernel_server()
{
    accept_connections = false;
    pthread_join(thr_server_conn, NULL);
    socket_destroy(server_socket);
    free(connection);
    log_info(logger_aux, "Thread Kernel Server: finalizado");
}

void *process_client_entry(void *ptr)
{
    if (!ptr)
        pthread_exit(0);
    t_client_connection *conn = (t_client_connection *)ptr;
    // if (hs_server_to_module_valid(conn->socket, HSKERNEL, logger_aux))
    log_info(logger_aux, "Thread con PID: %d iniciado", conn->pid);
    process_client_communication(conn);
    socket_destroy(conn->socket);
    log_info(logger_aux, "Thread con PID: %d finalizado", conn->pid);
    free(conn);
    pthread_exit(0);
}

// Comunicacion con el cliente

void process_client_communication(t_client_connection *conn)
{
    bool exit = false;
    t_package *package = get_package(conn->socket, logger_aux);
    switch (package->operation_code)
    {
    case INSTRUCCIONES:
        log_info(logger_aux, "Thread con PID: %d instrucciones recibidas", conn->pid);
        log_info(logger_aux, "Thread con PID: %d instrucciones size %d", conn->pid, package->buffer->size);
        t_list *instrucciones = get_instrucciones(package); // 800

        t_pcb *pcb = pcb_create(conn->pid, instrucciones);
        // send_instruccion(modules_client->memory_client_socket, "TABLA SEGMENTOS NUEVA", logger_aux);
        // t_package *package = get_package(modules_client->memory_client_socket, logger_aux);
        // t_tabla_segmentos *tabla_segmentos = get_tabla_segmentos(package);
        // pcb->segments_table = tabla_segmentos;
        list_add(queues->NEW, pcb);
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
    if (exit)
        return;
    // Search for the pcb pid in EXIT queue
    log_info(logger_aux, "Thread con PID: %d esperando a que termine", conn->pid);
    t_pcb *last = NULL;
    last = search_pid(queues->EXIT, conn->pid);
    while (last == NULL)
    {
        last = search_pid(queues->EXIT, conn->pid);
        sleep(1);
    }
    send_exit(conn->socket, logger_aux);
}

t_pcb *search_pid(t_list *queue, int pid)
{
    for (size_t i = 0; i < list_size(queue); i++)
    {
        t_pcb *pcb = list_get(queue, i);
        if (pcb->pid == pid)
            return pcb;
    }
    return NULL;
}