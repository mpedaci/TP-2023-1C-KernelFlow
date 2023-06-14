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
   while (server_fd != -1)
   { // No estoy muy seguro de esta condicion
      char *socket_client = string_itoa(client_wait(server_fd, logger_aux));
      pthread_t conection;
      pthread_create(&conection, 0, handle_client, (void *)socket_client);
      pthread_detach(conection);
   }
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
