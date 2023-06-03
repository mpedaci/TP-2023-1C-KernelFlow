#include "planner.h"

// FIFO
t_pcb *FIFO(t_list *queue)
{
    return list_remove(queue, 0);
}

// HRRN
// double est_sig_rafaga;
// t_temporal *tiempo_llegada_ready;

void check_est_sig_rafaga(t_pcb *pcb)
{
    if (pcb->est_sig_rafaga == 0)
        pcb->est_sig_rafaga = config_kernel->estimacion_inicial;
}

double calculate_est_sig_rafaga(t_pcb *pcb)
{
    // check_est_sig_rafaga(pcb);
    // alpha * t_ultima_rafa_real + (1 - alpha) * est_ant_rafaja
    // int64_t rn = temporal_diff(pcb->tiempo_salida_cpu, pcb->tiempo_entrada_cpu);
    int64_t rn = temporal_diff(pcb->tiempo_entrada_cpu, pcb->tiempo_salida_cpu);
    return config_kernel->hrrn_alfa * rn + (1 - config_kernel->hrrn_alfa) * pcb->est_sig_rafaga;
}

void update_est_sig_rafaga(t_pcb *pcb)
{
    pcb->est_sig_rafaga = calculate_est_sig_rafaga(pcb);
}

double calculate_rr(t_pcb *pcb, t_temporal *tiempo_actual)
{
    int64_t s = temporal_diff(pcb->tiempo_llegada_ready, tiempo_actual);
    double w = pcb->est_sig_rafaga;
    return (s + w) / w;
}

bool sort_by_rr(t_pcb *pcb1, t_pcb *pcb2)
{
    t_temporal *tiempo_actual = temporal_create();
    double rr1 = calculate_rr(pcb1, tiempo_actual);
    double rr2 = calculate_rr(pcb2, tiempo_actual);
    // log_info(logger_main, "PID %d | RR1: %f", pcb1->pid, rr1);
    // log_info(logger_main, "PID %d | RR2: %f", pcb2->pid, rr2);
    return rr1 >= rr2;
}

/* ORDENA LA COLA POR HRRN Y DEVUELVE EL ELEMENTO A PROCESAR */
t_pcb *HRRN(t_list *queue)
{
    log_info(logger_main, "Planificando por HRRN");
    // ORDENAR QUEUE POR HRRN
    list_sort(queue, (void *)sort_by_rr);

    t_temporal *tiempo_actual = temporal_create();
    for (int i = 0; i < list_size(queue); i++)
    {
        t_pcb *pcb = list_get(queue, i);
        log_info(logger_main, "PID %d | RR: %f", pcb->pid, calculate_rr(pcb, tiempo_actual));
    }

    // DEVOLVER PRIMER ELEMENTO
    t_pcb *pcb = list_remove(queue, 0);
    return pcb;
}