#ifndef SERVER_HANDLE_H
#define SERVER_HANDLE_H

#include <stdbool.h>
#include <pthread.h>

#include "types.h"
#include "kernel_vars.h"
#include "kernel_structs.h"

#include "utils.h"
#include "server.h"
#include "communication.h"


void start_kernel_server(char *listen_port);
void end_kernel_server();

void *process_client_entry(void *ptr);

void process_client_communication(t_client_connection *conn);

#endif /* SERVER_HANDLE_H */
