#ifndef QUEUE_CONTROLLER_H
#define QUEUE_CONTROLLER_H

#include "types.h"
#include "kernel_vars.h"

pthread_mutex_t get_queue_mutex(t_queue_id queue_id);
t_list *get_queue(t_queue_id queue_id);
void add_pcb_to_queue(t_queue_id queue_id, t_pcb *pcb);
t_pcb *pop_pcb_from_queue(t_queue_id queue_id);
t_pcb *pop_pcb_from_queue_by_index(t_queue_id queue_id, int index);
bool is_queue_empty(t_queue_id queue_id);
t_pcb *remove_pcb_from_queue_resourse(t_recurso *recurso);

#endif /* QUEUE_CONTROLLER_H */