#include "planner.h"

void apply_planner_algorithm()
{
    if (algorithm_is("FIFO"))
        FIFO(queues->READY);
    else if (algorithm_is("HRRN"))
        HRRN(queues->READY);
    else
        log_error(logger_main, "No se reconoce el algoritmo de planificacion");
    char *pids_string = get_pids_queue_string(queues->READY);
    log_info(logger_main, "Cola Ready %s: %s", config_kernel->algoritmo_planificacion, pids_string);
    free(pids_string);
}

bool algorithm_is(char *algorithm)
{
    return string_equals_ignore_case(config_kernel->algoritmo_planificacion, algorithm);
}

// FIFO

void FIFO(t_queue *queue)
{
    return;
}

// HRRN

void HRRN(t_queue *queue)
{
    pthread_mutex_lock(&queue->mutex);
    list_sort(queue->queue, (void *)sort_by_rr);
    pthread_mutex_unlock(&queue->mutex);
}

bool sort_by_rr(t_pcb *pcb1, t_pcb *pcb2)
{
    t_temporal *tiempo_actual = temporal_create();
    double rr1 = calculate_rr(pcb1, tiempo_actual);
    double rr2 = calculate_rr(pcb2, tiempo_actual);
    return rr1 >= rr2;
}

double calculate_rr(t_pcb *pcb, t_temporal *tiempo_actual)
{
    int64_t s = temporal_diff(pcb->tiempo_llegada_ready, tiempo_actual);
    double w = pcb->est_sig_rafaga;
    return (s + w) / w;
}
