#ifndef CONSOLA_H
#define CONSOLA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "client_handle.h"

// includes del modulo
#include "parser.h"


void start_console_client(char* ip_kernel, char* puerto_kernel, t_log* logger_console);


t_log* logger_console; 
t_log* logger_aux;

char* config_console_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/consola/config/console.config";
char* logger_console_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/consola/consola.log";
char* logger_aux_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/consola/consola_aux.log";

#endif