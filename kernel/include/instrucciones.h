#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

#include <commons/string.h>
#include <commons/collections/list.h>

#include "types.h"
#include "kernel_vars.h"
#include "kernel_structs.h"
#include "kernel_core.h"

#include "planner.h"
#include "communication.h"


bool execute_create_segment(uint32_t segment_size, uint32_t segment_id, t_pcb *pcb);
bool execute_delete_segment(uint32_t segment_id, t_pcb *pcb);
bool execute_wait(char *recurso_solicitado, t_pcb *pcb);
bool execute_signal(char *recurso_solicitado, t_pcb *pcb);
void execute_io(int tiempo, t_pcb *pcb);
void execute_exit(t_pcb *pcb, char *motivo);
void execute_to_ready(t_pcb *pcb);

void compactar();

void *io(void *tiempo);

#endif /* INSTRUCCIONES_H */