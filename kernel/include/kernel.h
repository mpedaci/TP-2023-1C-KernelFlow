#ifndef KERNEL_H
#define KERNEL_H

#include <stdlib.h>
#include <signal.h> // Para cierre seguro

#include <commons/log.h>

#include "kernel_vars.h"


#include "utils.h"
#include "client_handle.h"
#include "server_handle.h"

char *config_path = "/home/utnso/Desktop/tp-2023-1c-KernelFlow/kernel/conf/kernel.config";
char *logger_main_path = "/home/utnso/Desktop/tp-2023-1c-KernelFlow/kernel/logs/kernel.log";
char *logger_aux_path = "/home/utnso/Desktop/tp-2023-1c-KernelFlow/kernel/logs/kernel_aux.log";

int main();
void sigintHandler(int signum); // Para cierre seguro

#endif /* KERNEL_H */