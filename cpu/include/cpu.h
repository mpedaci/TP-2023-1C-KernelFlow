#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "server_handle.h"
#include "client_handle.h"
#include "types.h"
#include "package.h"

t_log* logger;
t_log* logger_aux;

char* logger_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/cpu/log/cpu.log";
char* logger_aux_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/cpu/log_aux/cpu_aux.log";
char* config_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/cpu/conf/cpu.config";

t_registers* registers;

bool ejecutando = false;

#endif