#include "client_handle.h"

t_modules_client *start_modules_client(t_config_kernel *config, t_log *logger)
{
    t_modules_client *modules_client = malloc(sizeof(t_modules_client));
    modules_client->memory_client_socket = start_memory_client(config->ip_memoria, config->puerto_memoria, logger);
    modules_client->filesystem_client_socket = start_filesystem_client(config->ip_filesystem, config->puerto_filesystem, logger);
    modules_client->cpu_client_socket = start_cpu_client(config->ip_cpu, config->ip_cpu, logger);
    if (modules_client->memory_client_socket == -1 || modules_client->filesystem_client_socket == -1 || modules_client->cpu_client_socket == -1)
        return NULL;
    else
        return modules_client;
}

int start_memory_client(char *ip, char *port, t_log *logger)
{
    int client_socket = create_connection(ip, port, logger);
    if (client_socket == -1)
    {
        log_error(logger, "No se pudo conectar al servidor de memoria");
        return -1;
    }
    if (!hs_client_to_module_valid(client_socket, HSKERNEL, HSMEMORIA, logger))
        return -1;
    log_info(logger, "Conectado al servidor de memoria");
    return client_socket;
}

int start_filesystem_client(char *ip, char *port, t_log *logger)
{
    int client_socket = create_connection(ip, port, logger);
    if (client_socket == -1)
    {
        log_error(logger, "No se pudo conectar al servidor de filesystem");
        return -1;
    }
    if (!hs_client_to_module_valid(client_socket, HSKERNEL, HSFS, logger))
        return -1;
    log_info(logger, "Conectado al servidor de filesystem");
    return client_socket;
}

int start_cpu_client(char *ip, char *port, t_log *logger)
{
    int client_socket = create_connection(ip, port, logger);
    if (client_socket == -1)
        return -1;
    if (!hs_client_to_module_valid(client_socket, HSKERNEL, HSCPU, logger))
    {
        printf("No se pudo realizar el handshake\n");
        return -1;
    }
    log_info(logger, "Conectado al servidor de cpu");
    return client_socket;
}