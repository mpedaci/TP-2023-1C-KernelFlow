#ifndef PRINT_INFORMATION_H
#define PRINT_INFORMATION_H

#include <unistd.h>
#include <readline/readline.h> // Para prints

#include "types.h"
#include "kernel_vars.h"
#include "kernel_core.h"

// AUXILIAR

t_pcb *get_pcb_by_pid(int pid);

// PRINTS

void print_menu();
void print_config();
void print_recurso(char *recurso);
void print_instancias_recursos(char *instancias_recursos);
void print_queues();
void print_pcb_pid(t_pcb *pcb);
void print_pcb(t_pcb *pcb);
void print_archivos_abiertos(t_list *archivos_abiertos);
void print_recursos_compartidos(t_list *recursos_compartidos);
void print_segments(t_list *segments);
void print_registers(t_registers *registers);
void print_internal_states();
void print_status_recursos();
t_queue_id get_queue_id(t_pcb *pcb);
void kill_process(t_pcb *pcb_to_kill);

#endif /* PRINT_INFORMATION_H */