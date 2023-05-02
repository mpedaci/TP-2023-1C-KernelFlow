#ifndef SERVER_HANDLE_H
#define SERVER_HANDLE_H

#include "server.h"
#include "instruction_cycle.h"
#include "client_handle.h"


void start_cpu_server(char *listen_port, t_log *logger);

void process_client(int client_socket, t_log *logger);

void free_pcontexto(t_pcontexto* contexto);

void set_registers_cpu(t_registers *registers);

void set_registers_contexto(t_registers *registers);

#endif