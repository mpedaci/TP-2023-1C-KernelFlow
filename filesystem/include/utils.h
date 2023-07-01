#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <client.h> //agregado
#include <server.h> //agregado

#include "types.h"
#include "types_controllers.h"
#include "filesystem_vars.h"

t_config_filesystem *read_config(char *path, t_log *logger);
void end_program(t_log *logger_main, t_config_filesystem *config, t_log *logger_aux);
int start_memory_client(char *ip, char *port, t_log *logger);
// void handle_requests();

#endif /* UTILS_H */