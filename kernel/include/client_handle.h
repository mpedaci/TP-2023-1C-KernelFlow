#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H

#include "client.h"

#include "utils.h"

t_modules_client *start_modules_client(t_config_kernel *config, t_log *logger);

int start_memory_client(char *ip, char *port, t_log *logger);
int start_filesystem_client(char *ip, char *port, t_log *logger);
int start_cpu_client(char *ip, char *port, t_log *logger);

#endif /* CLIENT_HANDLE_H */
