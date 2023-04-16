#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "server.h"
#include "utils.h"

char *config_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/memoria/conf/memoria.config";
char *logger_main_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/memoria/logs/memoria.log";
char *logger_aux_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/memoria/logs/memoria_aux.log";

t_log *logger_main;
t_log *logger_aux;

#endif /* MEMORIA_H */