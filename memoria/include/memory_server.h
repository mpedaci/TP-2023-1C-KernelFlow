#ifndef MEMORY_SERVER_H
#define MEMORY_SERVER_H

#include "utils.h"
#include "modulos.h"

void start_memory_server(char *port);
void wait_clients(int server_fd);
void *handle_client(void *client_fd);

#endif /* MEMORY_SERVER_H */