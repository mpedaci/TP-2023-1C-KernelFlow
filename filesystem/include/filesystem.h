#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <stdio.h>
#include <stdlib.h> //agrego esta tmb pa el exit successs
#include <utils.h> //agrego los utils
#include <stdbool.h>
#include <archivo.h>
#include <handle_requests_kernel.h>
#include "filesystem_server.h"

//cambiar las rutas si uso la vm utnso

char *config_path = "./config/filesystem.config";
char *logger_main_path = "./logs/filesystem.log";
char *logger_aux_path = "./logs/filesystem_aux.log";
t_log *logger_main;
t_log *logger_aux;

void atender_request(int socket_kernel);

#endif /* FILESYSTEM_H */


