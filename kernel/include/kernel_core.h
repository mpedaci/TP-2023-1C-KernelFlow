#ifndef KERNEL_CORE_H
#define KERNEL_CORE_H

#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

#include <commons/string.h>
#include <commons/collections/list.h>

#include "types.h"
#include "kernel_vars.h"
#include "kernel_structs.h"
#include "queue_controller.h"

#include "planner.h"
#include "communication.h"

#include "instrucciones.h"

// CORE

void start_kernel_core();
void end_kernel_core();

// CONTROLER

void *process_queues();

// CONDICIONES

bool can_move_NEW_to_READY();
bool algorithm_is_FIFO();
bool algorithm_is_HRRN();
bool can_execute_process();

// sincronizacion de colas

void inicializar_mutex();

// PCB

t_pcontexto *create_pcontexto_from_pcb(t_pcb *pcb);
void update_pcb_from_pcontexto(t_pcb *pcb, t_pcontexto_desalojo *pcontexto);
void cargar_recursos();
void execute();
void procesar_motivo_desalojo(t_pcontexto_desalojo *pcontexto_response);

#endif /* KERNEL_CORE_H */