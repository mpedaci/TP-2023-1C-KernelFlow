#ifndef SERVER_HANDLE_H
#define SERVER_HANDLE_H

#include "types.h"
#include "server.h"

#include <pthread.h>

void start_kernel_server(char *listen_port, t_log *logger);

void process_client(int client_socket, t_log *logger);

#endif /* SERVER_HANDLE_H */
