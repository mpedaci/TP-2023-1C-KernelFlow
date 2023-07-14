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
    pthread_join(thr_server, NULL);
    socket_destroy(server_socket);
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
        t_list *instrucciones = get_instrucciones(package);
        process_instrucciones(conn, instrucciones);
        break;
    case END:
        log_info(logger_aux, "Thread con PID: %d Conexion Finalizada", conn->pid);
        exit = true;
        break;
    default:
        log_info(logger_aux, "Thread con PID: %d Operacion desconocida", conn->pid);
        exit = true;
        break;
    }
    package_destroy(package);
    if (exit)
        return;
    // Search for the pcb pid in EXIT queue
    wait_to_end_process(conn);
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

void process_instrucciones(t_client_connection *conn, t_list *instrucciones)
{
    t_pcb *pcb = pcb_create(conn->pid, instrucciones);
    pcb->est_sig_rafaga = config_kernel->estimacion_inicial;
    list_add(all_pcb, pcb);
    // Se avisa a Consola su PID asignado
    t_pid_status *pid_status = pid_status_create(conn->pid, PROCESS_NEW);
    send_pid_status(conn->socket, pid_status, logger_aux);
    pid_status_destroy(pid_status);
    // Se verifica si se puede agregar a la cola READY
    if (accept_new_process_in_READY())
    {
        add_pcb_to_queue(pcb, QNEW);
        move_NEW_to_READY();
    }
    else
    {
        add_pcb_to_queue(pcb, QNEW);
    }
}

void wait_to_end_process(t_client_connection *conn)
{
    t_pcb *last = NULL;
    last = search_pid(queues->EXIT->queue, conn->pid);
    while (last == NULL && end_program_flag == false)
    {
        last = search_pid(queues->EXIT->queue, conn->pid);
        sleep(1);
    }
    if (end_program_flag == true)
    {
        last = search_pid(all_pcb, conn->pid);
        last->exit_status = PROCESS_ABORTED;
    }
    t_pid_status *pid_status = pid_status_create(conn->pid, last->exit_status);
    send_pid_status(conn->socket, pid_status, logger_aux);
    pid_status_destroy(pid_status);
}