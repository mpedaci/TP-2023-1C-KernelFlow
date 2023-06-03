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

char *config_path = "./config/kernel.config";
char *logger_main_path = "./logs/kernel.log";
char *logger_aux_path = "./logs/kernel_aux.log";

int main();
void sigintHandler(int signum); // Para cierre seguro

void print_menu();
void print_config();
void print_recurso(char *recurso);
void print_instancias_recursos(char *instancias_recursos);
void print_queues();
void print_pcb(t_pcb *pcb);
void print_pcb_pid(t_pcb *pcb);
void print_registers(t_registers *registers);
void print_internal_states();
t_pcb *get_pcb_by_pid(int pid);

#endif /* KERNEL_H */