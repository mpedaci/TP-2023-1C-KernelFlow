#include "queue_controller.h"

t_queue *get_queue(t_queue_id qId)
{
    switch (qId)
    {
    case QNEW:
        return queues->NEW;
    case QREADY:
        return queues->READY;
    case QEXEC:
        return queues->EXEC;
    case QBLOCK:
        return queues->BLOCK;
    case QEXIT:
        return queues->EXIT;
    default:
        return NULL;
    }
}

char *get_queue_name(t_queue_id qId)
{
    switch (qId)
    {
    case QNEW:
        return "NEW";
    case QREADY:
        return "READY";
    case QEXEC:
        return "EXEC";
    case QBLOCK:
        return "BLOCK";
    case QEXIT:
        return "EXIT";
    default:
        return NULL;
    }
}

int get_pcb_index_from_queue(t_pcb *pcb, t_queue *q)
{
    for (int i = 0; i < list_size(q->queue); i++)
    {
        t_pcb *p = list_get(q->queue, i);
        if (p->pid == pcb->pid)
            return i;
    }
    return -1;
}

t_pcb *move_fist_from_to(t_queue_id qSource, t_queue_id qDestiny)
{
    t_pcb *pcb_to_move = NULL;
    t_queue *source = get_queue(qSource);

    pthread_mutex_lock(&source->mutex);
    if (list_is_empty(source->queue))
    {
        pthread_mutex_unlock(&source->mutex);
        return NULL;
    }
    else
    {
        pcb_to_move = list_get(source->queue, 0);
    }
    pthread_mutex_unlock(&source->mutex);

    move_pcb_from_to(pcb_to_move, qSource, qDestiny);
    return pcb_to_move;
}

void move_pcb_from_to(t_pcb *pcb_to_move, t_queue_id qSource, t_queue_id qDestiny)
{
    t_queue *source = get_queue(qSource);
    t_queue *destiny = get_queue(qDestiny);

    pthread_mutex_lock(&source->mutex);
    pthread_mutex_lock(&destiny->mutex);

    int indexSource = get_pcb_index_from_queue(pcb_to_move, source);
    list_remove(source->queue, indexSource);

    list_add(destiny->queue, pcb_to_move);

    if (qDestiny == QREADY)
    {
        temporal_destroy(pcb_to_move->tiempo_llegada_ready);
        pcb_to_move->tiempo_llegada_ready = temporal_create();
    }

    pthread_mutex_unlock(&destiny->mutex);
    pthread_mutex_unlock(&source->mutex);

    log_info(logger_main, "PID: %d - Estado Anterior: %s - Estado Actual: %s", pcb_to_move->pid, get_queue_name(qSource), get_queue_name(qDestiny));
}

t_pcb *get_pcb_from(t_queue_id qSource)
{
    t_queue *source = get_queue(qSource);
    pthread_mutex_lock(&source->mutex);
    t_pcb *pcb_to_move = list_get(source->queue, 0);
    pthread_mutex_unlock(&source->mutex);
    return pcb_to_move;
}

void add_pcb_to_queue(t_pcb *pcb_to_move, t_queue_id qDestiny)
{
    t_queue *destiny = get_queue(qDestiny);
    pthread_mutex_lock(&destiny->mutex);
    list_add(destiny->queue, pcb_to_move);
    if (qDestiny == QNEW)
        log_info(logger_main, "Se crea el proceso %d en NEW", pcb_to_move->pid);
    pthread_mutex_unlock(&destiny->mutex);
}

bool is_queue_empty(t_queue_id qId)
{
    t_queue *q = get_queue(qId);
    pthread_mutex_lock(&q->mutex);
    bool isEmpty = (list_size(q->queue) == 0);
    pthread_mutex_unlock(&q->mutex);
    return isEmpty;
}

int get_queue_size(t_queue_id qId)
{
    t_queue *q = get_queue(qId);
    pthread_mutex_lock(&q->mutex);
    int size = list_size(q->queue);
    pthread_mutex_unlock(&q->mutex);
    return size;
}
