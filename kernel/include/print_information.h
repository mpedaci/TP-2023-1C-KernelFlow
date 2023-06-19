#ifndef PRINT_INFORMATION_H
#define PRINT_INFORMATION_H

#include <unistd.h>
#include <readline/readline.h> // Para prints

#include "types.h"
#include "kernel_vars.h"

void print_menu();
t_pcb *get_pcb_by_pid(int pid);
void print_config();
void print_recurso(char *recurso);
void print_instancias_recursos(char *instancias_recursos);
void print_queues();
void print_pcb_pid(t_pcb *pcb);
void print_pcb(t_pcb *pcb);
void print_registers(t_registers *registers);
void print_internal_states();
void print_status_recursos();
void print_segments(t_list *segments);

#endif /* PRINT_INFORMATION_H */