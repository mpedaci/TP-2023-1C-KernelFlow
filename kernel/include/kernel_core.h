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
#include "kernel_sources.h"
#include "queue_controller.h"

#include "kernel_cycle.h"

#include "utils.h"

#include "planner.h"
#include "communication.h"

#include "kernel_instructions.h"

// CORE

void start_kernel_core();

// CONTROLLER

void end_kernel_core();
void *process_queues();

// STATUS MOVES

void move_NEW_to_READY();
void move_READY_to_EXEC();

// CONDICIONES

int get_actual_multiprog();
bool accept_new_process_in_READY();
bool can_move_NEW_to_READY();
bool can_execute_process();

#endif /* KERNEL_CORE_H */