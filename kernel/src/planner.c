#include "planner.h"

// FIFO

t_pcb *FIFO(t_list *queue)
{
    return list_remove(queue, 0);
}

// HRRN

/* ORDENA LA COLA POR HRRN Y DEVUELVE EL ELEMENTO A PROCESAR */
t_pcb *HRRN(t_list *queue)
{
    // list_sort(t_list *, bool (*comparator)(void *, void *));
    return queue;
}