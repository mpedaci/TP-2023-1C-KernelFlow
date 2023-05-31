#ifndef KERNEL_CORE_H
#define KERNEL_CORE_H

#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include <commons/string.h>
#include <commons/collections/list.h>

#include "types.h"
#include "kernel_vars.h"
#include "kernel_structs.h"

#include "planner.h"
#include "communication.h"

#include "semaphore.h"
#include "instrucciones.h"


// CORE

void start_kernel_core();
void end_kernel_core();

// CONTROLER

void *process_queues();

// MOVIMIENTO DE COLAS

void queue_add(t_list *destiny, t_pcb *pcb);
void queue_remove(t_list *origin, t_pcb *pcb);
t_pcb *queue_pop(t_list *origin);
void queue_push(t_list *destiny, t_pcb *pcb);
bool queue_is_empty(t_list *queue);

// CONDICIONES

bool can_move_NEW_to_READY();
bool algorithm_is_FIFO();
bool algorithm_is_HRRN();
bool can_execute_process();

// PCB

t_pcontexto *create_pcontexto_from_pcb(t_pcb *pcb);
void update_pcb_from_pcontexto(t_pcb *pcb, t_pcontexto *pcontexto);
void execute_process();

//Mutex para proteger las colas
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_running;
pthread_mutex_t mutex_blocked;
pthread_mutex_t mutex_exit;
pthread_mutex_t mutex_pid;

void inicializar_mutex();

// sincronizacion de colas 

void agregar_pcb_a_cola(t_pcb* pcb, pthread_mutex_t mutex, t_list* cola);
t_pcb* quitar_pcb_de_cola(pthread_mutex_t mutex, t_list* cola);

void agregar_pcb_a_lista(t_pcb* pcb, pthread_mutex_t mutex, t_list* cola);
t_pcb* quitar_primer_pcb_de_lista(pthread_mutex_t mutex, t_list* cola);


#endif /* KERNEL_CORE_H */