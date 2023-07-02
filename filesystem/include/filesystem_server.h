#ifndef FILESYSTEM_SERVER_H
#define FILESYSTEM_SERVER_H

#include <pthread.h>

#include "server.h"
#include "filesystem_vars.h"
#include "filesystem_instructions.h"
#include "types_controllers.h"

#include <commons/collections/list.h>

void start_filesystem_server();
void process_client(int client_socket);
void handle_instruccion(t_instruccion *instruccion, int client_socket);
void process_request_async(t_inst_client *icr);

#endif /* FILESYSTEM_SERVER_H */