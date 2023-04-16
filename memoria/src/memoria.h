#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "server.h"

void iterator(char* value);
t_config* start_config(void);
void finish_program(t_log* logger,t_config* config, int socket_server, int socket_client);

#endif /* MEMORIA_H */