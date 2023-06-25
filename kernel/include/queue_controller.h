#ifndef QUEUE_CONTROLLER_H
#define QUEUE_CONTROLLER_H

#include "types.h"
#include "kernel_vars.h"
#include "planner.h"

t_queue *get_queue(t_queue_id qId);
char *get_queue_name(t_queue_id qId);
int get_pcb_index_from_queue(t_pcb *pcb, t_queue *q);
t_pcb *move_fist_from_to(t_queue_id qSource, t_queue_id qDestiny);
void move_pcb_from_to(t_pcb *pcb_to_move, t_queue_id qSource, t_queue_id qDestiny);
t_pcb *get_pcb_from(t_queue_id qSource);
void add_pcb_to_queue(t_pcb *pcb_to_move, t_queue_id qDestiny);
bool is_queue_empty(t_queue_id qId);
int get_queue_size(t_queue_id qId);

#endif /* QUEUE_CONTROLLER_H */