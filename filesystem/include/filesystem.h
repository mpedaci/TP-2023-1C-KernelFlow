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
char *logger_main_path = "./logs/filesystem.log";;
char *logger_aux_path = "./logs/filesystem_aux.log";

int main();

#endif /* FILESYSTEM_H */


