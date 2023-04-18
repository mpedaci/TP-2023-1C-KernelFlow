#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <stdio.h>
#include <stdlib.h> //agrego esta tmb pa el exit successs
#include <utils.h> //agrego los utils
#include <stdbool.h>


//cambiar las rutas si uso la vm utnso

char *config_path = "/home/meli/Escritorio/tp-2023-1c-KernelFlow/filesystem/conf/filesystem.config";
char *logger_main_path = "/home/meli/Escritorio/tp-2023-1c-KernelFlow/filesystem/filesystem.log";
char *logger_aux_path = "/home/meli/Escritorio/tp-2023-1c-KernelFlow/filesystem/filesystem_aux.log";


t_log *logger_main;
t_log *logger_aux;


#endif /* FILESYSTEM_H */


