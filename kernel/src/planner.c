#include "planner.h"

void apply_planner_algorithm()
{
    if (algorithm_is("FIFO"))
        FIFO(queues->READY);
    else if (algorithm_is("HRRN"))
        HRRN(queues->READY);
    else
        log_error(logger_main, "No se reconoce el algoritmo de planificacion - utilizando FIFO");
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
    list_sort(queue->queue, (void *)sort_by_rr);
}

bool sort_by_rr(t_pcb *pcb1, t_pcb *pcb2)
{
    t_temporal *tiempo_actual = temporal_create();
    double rr1 = calculate_rr(pcb1, tiempo_actual);
    double rr2 = calculate_rr(pcb2, tiempo_actual);
    temporal_destroy(tiempo_actual);
    return rr1 >= rr2;
}

double calculate_rr(t_pcb *pcb, t_temporal *tiempo_actual)
{
    int64_t w = temporal_diff(pcb->tiempo_llegada_ready, tiempo_actual);
    double s = calculate_s(pcb, tiempo_actual);
    return (s + w) / s;
}

double calculate_s(t_pcb *pcb, t_temporal *tiempo_actual)
{
    int64_t raf_anterior = temporal_diff(pcb->tiempo_entrada_cpu, pcb->tiempo_salida_cpu);
    double s = config_kernel->hrrn_alfa * pcb->est_sig_rafaga + (1 - config_kernel->hrrn_alfa) * raf_anterior;
    return s;
}