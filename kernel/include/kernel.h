#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "server_handle.h"
#include "client_handle.h"


char *config_path = "/home/utnso/Desktop/tp-2023-1c-KernelFlow/kernel/conf/kernel.config";
char *logger_main_path = "/home/utnso/Desktop/tp-2023-1c-KernelFlow/kernel/logs/kernel.log";
char *logger_aux_path = "/home/utnso/Desktop/tp-2023-1c-KernelFlow/kernel/logs/kernel_aux.log";

t_log *logger_main;
t_log *logger_aux;

#endif /* KERNEL_H */