#include "memory_server.h"

t_client_connection *connection_info;

bool fs_connected = false;
bool cpu_connected = false;
bool kernel_connected = false;

void start_memory_server(char *port)
{
    log_info(logger_aux, "Iniciando servidor");
    int server_fd = server_start(port, logger_aux);
    if (server_fd == -1)
    {
        log_error(logger_aux, "No se pudo iniciar el servidor");
    }
    else
    {
        log_info(logger_aux, "Servidor listo para recibir al cliente");
        accept_connections = true;
        wait_clients(server_fd);
    }
    socket_destroy(server_fd);
}

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

void destroy_connection_info(t_client_connection *connection_info)
{
    socket_destroy(connection_info->socket);
    free(connection_info);
}

void wait_clients(int server_fd)
{
    bool close = false;

    while (accept_connections)
    { // No estoy muy seguro de esta condicion
        if (!fs_connected || !cpu_connected || !kernel_connected)
        {
            connection_info = (t_client_connection *)malloc(sizeof(t_client_connection));
            connection_info->socket = client_wait(server_fd, logger_aux);
            if (connection_info->socket != -1)
            {
                connection_info->HS_module = hs_server_to_module_get_type(connection_info->socket, HSMEMORIA, logger_aux);
                if (accept_new_module_connnection(connection_info))
                {
                    close = verify_existence_module();
                    if (!close)
                        pthread_create(&connection, 0, handle_client, (void *)connection_info);
                    else
                    {
                        close = false;
                        destroy_connection_info(connection_info);
                    }
                }
                else
                {
                    log_warning(logger_aux, "Cliente desconocido");
                    destroy_connection_info(connection_info);
                }
            }
            else
                free(connection_info);
        }
    }
    pthread_join(connection, NULL);
}

void *handle_client(void *conn_info)
{
    if (!conn_info)
        pthread_exit(0);
    t_client_connection *conn = (t_client_connection *)conn_info;

    switch (conn->HS_module)
    {
    case HSKERNEL:
        log_info(logger_aux, "Se conecto el modulo Kernel");
        kernel_operations(conn->socket);
        break;
    case HSCPU:
        log_info(logger_aux, "Se conecto el modulo CPU");
        cpu_operations(conn->socket);
        break;
    case HSFS:
        log_info(logger_aux, "Se conecto el modulo File System");
        fs_operations(conn->socket);
        break;
    default:
        log_warning(logger_aux, "Cliente desconocido");
        break;
    }
    // free(socket_client);
    destroy_connection_info(conn);
    pthread_exit(0);
}