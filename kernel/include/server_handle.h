#ifndef SERVER_HANDLE_H
#define SERVER_HANDLE_H

#include "types.h"
#include "server.h"

#include <pthread.h>

void start_kernel_server_one_thread(char *listen_port, t_log *logger);
void start_kernel_server(char *listen_port, t_log *logger);

void *process_client_entry(void *ptr);

void process_client_communication(int client_socket, int pid, t_log *logger);

typedef struct
{
    int sock;
    t_log *logger;
    int pid;
} t_client_connection;

#endif /* SERVER_HANDLE_H */
