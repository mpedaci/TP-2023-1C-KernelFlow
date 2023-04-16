#ifndef CONSOLA_H
#define CONSOLA_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>

//includes de libs
//#include "server.h"
#include "client.h"
#include "utils.h"

// includes del modulo
#include "parser.h"

t_config* initialize_config_console(char* config_path);
void destroy_console(t_config* config_console, t_log* logger_console, int kernel_socket);



#endif