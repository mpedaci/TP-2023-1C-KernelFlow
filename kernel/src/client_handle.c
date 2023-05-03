#include "client_handle.h"

t_modules_client *start_modules_client(t_config_kernel *config, t_log *logger)
{
    t_modules_client *modules_client = malloc(sizeof(t_modules_client));
    modules_client->memory_client_socket = start_client(config->ip_memoria, config->puerto_memoria, HSMEMORIA, logger);
    modules_client->cpu_client_socket = start_client(config->ip_cpu, config->puerto_cpu, HSCPU, logger);
    modules_client->filesystem_client_socket = start_client(config->ip_filesystem, config->puerto_filesystem, HSFS, logger);
    if (modules_client->memory_client_socket == -1 || modules_client->filesystem_client_socket == -1 || modules_client->cpu_client_socket == -1){
        free(modules_client);
        return NULL;
    }
    else
        return modules_client;
}

int start_client(char *ip, char *port, hs_code hsmodule, t_log *logger)
{
    int client_socket = create_connection(ip, port, logger);
    if (client_socket == -1)
        return -1;
    if (!hs_client_to_module_valid(client_socket, HSKERNEL, hsmodule, logger))
        return -1;
    return client_socket;
}