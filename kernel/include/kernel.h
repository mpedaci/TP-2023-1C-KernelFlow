#ifndef KERNEL_H
#define KERNEL_H

#include <stdlib.h>
#include <signal.h> // Para cierre seguro

#include <commons/log.h>
#include <commons/collections/list.h>

#include "kernel_vars.h"
#include "print_information.h"

#include "utils.h"
#include "client_handle.h"
#include "server_handle.h"
#include "kernel_core.h"

char *config_path = "./config/kernel.config";
char *logger_main_path = "./logs/kernel.log";
char *logger_aux_path = "./logs/kernel_aux.log";

int main();
void sigintHandler(int signum); // Para cierre seguro

#endif /* KERNEL_H */