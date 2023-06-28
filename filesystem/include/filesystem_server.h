#ifndef FILESYSTEM_SERVER_H
#define FILESYSTEM_SERVER_H

#include "server.h"

void start_filesystem_server(char *listen_port, t_log *logger);
void process_client(int client_socket, t_log *logger);
void handle_instruccion(t_instruccion *instruccion, int client_socket, t_log *logger);

#endif /* FILESYSTEM_SERVER_H */