#ifndef MEMORY_SERVER_H
#define MEMORY_SERVER_H

#include "utils.h"
#include "modulos.h"

void start_memory_server(char* port,t_log* logger);
void wait_clients(int server_fd,t_log* logger);
void handle_client(int client_fd,t_log* logger);

#endif /* MEMORY_SERVER_H */