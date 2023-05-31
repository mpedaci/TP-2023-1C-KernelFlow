#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include <commons/string.h>
#include <commons/collections/list.h>

#include "types.h"
#include "kernel_vars.h"
#include "kernel_structs.h"
#include "kernel_core.h"

#include "planner.h"
#include "communication.h"

#include "semaphore.h"
void execute_create_segment(uint32_t segment_size, uint32_t segment_id, t_pcb* pcb);
void execute_delete_segment(uint32_t id_segment, t_pcb* pcb);
void execute_wait(char* recurso_solicitado, t_recurso** recursos, t_pcb *pcb);
void execute_signal(char* recurso_solicitado, t_recurso** recursos, t_pcb *pcb);
void execute_io(int tiempo, t_pcb* pcb);

#endif  INSTRUCCIONES_H 