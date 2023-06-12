#include "queue_controller.h"

pthread_mutex_t get_queue_mutex(t_queue_id queue_id)
{
    switch (queue_id)
    {
    case QNEW:
        return mutex_new;
    case QREADY:
        return mutex_ready;
    case QEXEC:
        return mutex_running;
    case QBLOCK:
        return mutex_blocked;
    case QEXIT:
        return mutex_exit;
    }
    return mutex_exit;
}

t_list *get_queue(t_queue_id queue_id)
{
    switch (queue_id)
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

void add_pcb_to_queue(t_queue_id queue_id, t_pcb *pcb)
{
    pthread_mutex_t mutex = get_queue_mutex(queue_id);
    t_list *cola = get_queue(queue_id);
    pthread_mutex_lock(&mutex);
    list_add(cola, pcb);
    pthread_mutex_unlock(&mutex);
}

t_pcb *pop_pcb_from_queue(t_queue_id queue_id)
{
    pthread_mutex_t mutex = get_queue_mutex(queue_id);
    t_list *cola = get_queue(queue_id);
    pthread_mutex_lock(&mutex);
    t_pcb *pcb = list_remove(cola, 0);
    pthread_mutex_unlock(&mutex);
    return pcb;
}

t_pcb *pop_pcb_from_queue_by_index(t_queue_id queue_id, int index)
{
    pthread_mutex_t mutex = get_queue_mutex(queue_id);
    t_list *cola = get_queue(queue_id);
    pthread_mutex_lock(&mutex);
    t_pcb *pcb = list_remove(cola, index);
    pthread_mutex_unlock(&mutex);
    return pcb;
}

bool is_queue_empty(t_queue_id queue_id)
{
    pthread_mutex_t mutex = get_queue_mutex(queue_id);
    t_list *cola = get_queue(queue_id);
    pthread_mutex_lock(&mutex);
    bool result = list_is_empty(cola);
    pthread_mutex_unlock(&mutex);
    return result;
}

t_pcb *remove_pcb_from_queue_resourse(t_recurso *recurso)
{
    pthread_mutex_lock(&recurso->mutex);
    t_pcb *pcb = list_remove(recurso->lista_bloqueados, 0);
    pthread_mutex_unlock(&recurso->mutex);
    return pcb;
}