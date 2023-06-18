#ifndef MEMORY_SERVER_H
#define MEMORY_SERVER_H

#include "modulos.h"

void start_memory_server(char *listen_port);
void *start_server_listen(void *listen_port);
void end_memory_server();
void *process_client_entry(void *ptr);
bool accept_new_module_connnection(t_client_connection *conn);
bool verify_existence_module();
void destroy_connection_info(t_client_connection *connection_info);

#endif /* MEMORY_SERVER_H */