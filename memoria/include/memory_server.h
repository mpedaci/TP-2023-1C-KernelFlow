#ifndef MEMORY_SERVER_H
#define MEMORY_SERVER_H

#include "utils.h"
#include "modulos.h"

typedef struct{
    int socket_client;
    t_log* log;
} t_arguments;

void start_memory_server(char* port,t_log* logger);
void wait_clients(int server_fd,t_log* logger);
void* handle_client(void* arguments);

#endif /* MEMORY_SERVER_H */