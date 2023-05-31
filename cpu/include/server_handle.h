#ifndef SERVER_HANDLE_H
#define SERVER_HANDLE_H

#include "server.h"
#include "instruction_cycle.h"

void start_cpu_server(char *listen_port, t_log *logger);

void process_client(int client_socket, t_log *logger);

void free_pcontexto(t_pcontexto *contexto);

void free_pcontexto_desalojo(t_pcontexto_desalojo *contexto);

#endif