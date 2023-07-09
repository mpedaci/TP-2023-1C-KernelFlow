#include "memory_server.h"

t_client_connection *connection_info;

void start_memory_server(char *listen_port)
{
    pthread_create(&thr_server, 0, start_server_listen, (void *)listen_port);
}

void *start_server_listen(void *listen_port)
{
    bool reject = true;
    char *listen_port_aux = (char *)listen_port;
    server_socket = server_start(listen_port_aux, logger_aux);
    if (server_socket == -1)
    {
        accept_connections = false;
        end_program_flag = true;
        pthread_exit(0);
    }
    while (accept_connections)
    {
        if (!fs_connected || !cpu_connected || !kernel_connected)
        {
            connection_info = (t_client_connection *)malloc(sizeof(t_client_connection));
            connection_info->socket = client_wait(server_socket, logger_aux);
            if (connection_info->socket != -1)
            {
                connection_info->HS_module = hs_server_to_module_get_type(connection_info->socket, HSMEMORIA, logger_aux);
                if (accept_new_module_connnection(connection_info))
                {
                    reject = verify_existence_module();
                    if (!reject)
                        pthread_create(&thr_server_conn, 0, process_client_entry, (void *)connection_info);
                } else {
                    reject = true;
                    socket_destroy(connection_info->socket);
                    free(connection_info);
                }
            } else {
                free(connection_info);
            }
        }
        if (fs_connected && cpu_connected && kernel_connected)
        {
            accept_connections = false;
            log_info(logger_aux, "Thread Memory Server: Se conectaron todos los modulos");
        }
    }
    pthread_exit(0);
}

void end_memory_server()
{
    accept_connections = false;
    pthread_join(thr_server_conn, NULL);
    pthread_join(thr_server, NULL);
    if (server_socket != -1)
        socket_destroy(server_socket);
    log_info(logger_aux, "Thread Memory Server: finalizado");
}

void *process_client_entry(void *ptr)
{
    if (!ptr)
        pthread_exit(0);
    t_client_connection *conn = (t_client_connection *)ptr;
    switch (conn->HS_module)
    {
    case HSKERNEL:
        log_info(logger_aux, "Thread Memory Client: Se conecto el modulo Kernel");
        kernel_operations(conn->socket);
        log_info(logger_aux, "Thread Memory Client: Se desconectara el modulo Kernel");
        break;
    case HSCPU:
        log_info(logger_aux, "Thread Memory Client: Se conecto el modulo CPU");
        cpu_fs_operations(conn->socket, "CPU");
        log_info(logger_aux, "Thread Memory Client: Se desconectara el modulo CPU");
        break;
    case HSFS:
        log_info(logger_aux, "Thread Memory Client: Se conecto el modulo File System");
        cpu_fs_operations(conn->socket, "Filesystem");
        log_info(logger_aux, "Thread Memory Client: Se desconectara el modulo File System");
        break;
    default:
        log_warning(logger_aux, "Thread Memory Client: Cliente desconocido");
        break;
    }
    socket_destroy(conn->socket);
    free(conn);
    pthread_exit(0);
}

// Condiciones

bool accept_new_module_connnection(t_client_connection *conn)
{
    return connection_info->HS_module != -1 && (connection_info->HS_module == HSKERNEL || connection_info->HS_module == HSCPU || connection_info->HS_module == HSFS);
}

bool verify_existence_module()
{
    switch (connection_info->HS_module)
    {
    case HSKERNEL:
        if (!kernel_connected)
            kernel_connected = true;
        else
            return true;
        break;
    case HSCPU:
        if (!cpu_connected)
            cpu_connected = true;
        else
            return true;
        break;
    case HSFS:
        if (!fs_connected)
            fs_connected = true;
        else
            return true;
        break;
    default:
        return true;
        break;
    }
    return false;
}

// Destructor
void destroy_connection_info(t_client_connection *connection_info)
{
    if (connection_info->socket != -1)
        socket_destroy(connection_info->socket);
    free(connection_info);
}