#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H

#include "client.h"

void start_memory_client(char *ip, char *port, t_log *logger);
void start_filesystem_client(char *ip, char *port, t_log *logger);
void start_cpu_client(char *ip, char *port, t_log *logger);

#endif /* CLIENT_HANDLE_H */
