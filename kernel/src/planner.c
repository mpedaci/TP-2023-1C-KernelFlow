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
    check_est_sig_rafaga(pcb);
    // alpha * est_ant_rafaja + (1 - alpha) * est_ant_rafaja
    return config_kernel->hrrn_alfa * pcb->est_sig_rafaga + (1 - config_kernel->hrrn_alfa) * pcb->est_sig_rafaga;
}

void update_est_sig_rafaga(t_pcb *pcb)
{
    pcb->est_sig_rafaga = calculate_est_sig_rafaga(pcb);
}

double calculate_rr(t_pcb *pcb)
{
    int64_t time_ready = temporal_diff(temporal_create(), pcb->tiempo_llegada_ready);
    return (time_ready + calculate_est_sig_rafaga(pcb)) / calculate_est_sig_rafaga(pcb);
}

bool sort_by_rr(t_pcb *pcb1, t_pcb *pcb2)
{
    return calculate_rr(pcb1) > calculate_rr(pcb2);
}

/* ORDENA LA COLA POR HRRN Y DEVUELVE EL ELEMENTO A PROCESAR */
t_pcb *HRRN(t_list *queue)
{
    // ORDENAR QUEUE POR HRRN
    list_sort(queue, (void *)sort_by_rr);
    // DEVOLVER PRIMER ELEMENTO
    t_pcb *pcb = list_remove(queue, 0);
    update_est_sig_rafaga(pcb);
    return pcb;
}