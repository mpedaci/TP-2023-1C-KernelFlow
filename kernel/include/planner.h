#ifndef PLANNER_H
#define PLANNER_H

#include <stdbool.h>

#include "types.h"
#include "kernel_structs.h"
#include "kernel_vars.h"

#include "utils.h"

void apply_planner_algorithm();
bool algorithm_is(char *algorithm);

void FIFO(t_queue *queue);

void HRRN(t_queue *queue);
bool sort_by_rr(t_pcb *pcb1, t_pcb *pcb2);
double calculate_rr(t_pcb *pcb, t_temporal *tiempo_actual);
double calculate_s(t_pcb *pcb, t_temporal *tiempo_actual);

#endif /* PLANNER_H */
