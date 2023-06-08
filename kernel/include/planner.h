#ifndef PLANNER_H
#define PLANNER_H

#include <stdbool.h>

#include "types.h"
#include "kernel_structs.h"
#include "kernel_vars.h"

#include "utils.h"

// FIFO
t_pcb *FIFO(t_list *queue);

// HRRN
void check_est_sig_rafaga(t_pcb *pcb);
double calculate_est_sig_rafaga(t_pcb *pcb);
void update_est_sig_rafaga(t_pcb *pcb);
double calculate_rr(t_pcb *pcb, t_temporal *tiempo_actual);
bool sort_by_rr(t_pcb *pcb1, t_pcb *pcb2);

t_pcb *HRRN(t_list *queue);

#endif /* PLANNER_H */
