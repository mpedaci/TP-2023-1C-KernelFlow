#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>


typedef struct{
    char* ip_kernel;
    char* puerto_kernel;

}t_config_console;

t_config_console* read_config(char* path, t_log* logger_console);
void end_program(t_log* logger_console, t_log* logger_aux, t_config_console* config);


#endif