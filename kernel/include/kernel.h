#ifndef KERNEL_H
#define KERNEL_H

#include <stdlib.h>
#include <signal.h> // Para cierre seguro
#include <readline/readline.h> // Para prints

#include <commons/log.h>
#include <commons/collections/list.h>

#include "kernel_vars.h"


#include "utils.h"
#include "client_handle.h"
#include "server_handle.h"
#include "kernel_core.h"

char *config_path = "/home/utnso/Desktop/tp-2023-1c-KernelFlow/kernel/conf/kernel.config";
char *logger_main_path = "/home/utnso/Desktop/tp-2023-1c-KernelFlow/kernel/logs/kernel.log";
char *logger_aux_path = "/home/utnso/Desktop/tp-2023-1c-KernelFlow/kernel/logs/kernel_aux.log";

int main();
void sigintHandler(int signum); // Para cierre seguro

void print_menu();
void print_config();
void print_recurso();
void print_instancias_recursos();
void print_queues();
void print_pcb();
void print_internal_states();

#endif /* KERNEL_H */