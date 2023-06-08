#ifndef SERVER_HANDLE_H
#define SERVER_HANDLE_H

#include <stdbool.h>
#include <pthread.h>

#include "types.h"
#include "kernel_vars.h"
#include "kernel_structs.h"
#include "queue_controller.h"

#include "utils.h"
#include "server.h"
#include "communication.h"


void start_kernel_server(char *listen_port);
void *start_server_listen(void *listen_port);
void end_kernel_server();

void *process_client_entry(void *ptr);

t_pcb *search_pid(t_list *queue, int pid);
void process_client_communication(t_client_connection *conn);

#endif /* SERVER_HANDLE_H */
