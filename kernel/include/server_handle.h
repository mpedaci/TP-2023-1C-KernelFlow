#ifndef SERVER_HANDLE_H
#define SERVER_HANDLE_H

#include "server.h"

#include <pthread.h>

void start_kernel_server(char *listen_port, t_log *logger);

void start_kernel_server_thread(char *listen_port, t_log *logger);
void atender_cliente(int socket_cliente);

void process_client(int client_socket, t_log *logger);
void iteratorFunction(char *value);

#endif /* SERVER_HANDLE_H */
