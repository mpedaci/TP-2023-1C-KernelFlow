#ifndef PLANNER_H
#define PLANNER_H

#include <stdbool.h>

#include "types.h"
#include "kernel_structs.h"

#include "utils.h"

t_pcb *FIFO(t_list *queue);

t_pcb *HRRN(t_list *queue);

#endif /* PLANNER_H */
