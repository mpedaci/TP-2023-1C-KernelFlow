#include "instrucciones.h"

void execute_create_segment(uint32_t segment_size, uint32_t segment_id, t_pcb *pcb)
{
    // Enviar a memoria tamanio para crear segmento y id
    /* send(modules_client->memory_client_socket, &segment_size, sizeof(uint32_t), NULL);
    send(modules_client->memory_client_socket, &segment_id, sizeof(uint32_t), NULL);

    op_code codigo_operacion = get_op_code(modules_client->memory_client_socket);       // falta desde memoria

    switch(codigo_operacion){
        case EXITOSO:
            uint32_t base_nuevo_segmento;
            recv(modules_client->memory_client_socket, &base_nuevo_segmento, sizeof(uint32_t), NULL);
            t_segment* nuevo_segmento = malloc(sizeof(t_segment));
            nuevo_segmento->base_address = base_nuevo_segmento;
            nuevo_segmento->size = segment_size;
            nuevo_segmento->id = segment_id;
            list_add(pcb->segments_table->segment_list, nuevo_segmento);
            log_info(logger_main,"PID: %d - Crear Segmento - Id: %d - Tamanio: %d", pcb->pid, segment_id, segment_size);
            break;

        case SIN_ESPACIO:
            log_error(logger_main, "OUTOFMEMORY"); //?
            agregar_pcb_a_cola(pcb, mutex_exit, queues->EXIT);
            break;

        case COMPACTAR:
            compactar();
            t_segments_table *tabla_actualizada = get_tsegmento(modules_client->memory_client_socket);
            pcb->segments_table = tabla_actualizada;

            // vuelvo a ejecutar create_segment
            execute_create_segment(segment_size, segment_id, pcb);
    }

    // Devolver contexto a cpu
    t_pcontexto *pcontexto = create_pcontexto_from_pcb(pcb);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux); */
}

void compactar()
{
    log_info(logger_main, "Compactacion: Se solicito compactacion");
    log_info(logger_main, "Compactacion: Esperando Fin de Operacion de FS");
    log_info(logger_main, "Se finalizo el proceso de compactacion");
}

void execute_delete_segment(uint32_t segment_id, t_pcb *pcb)
{
    // Enviar a memoria id del segmento a eliminar
    /* send(modules_client->memory_client_socket, &segment_id, sizeof(uint32_t), NULL);

    t_package* p = get_package(modules_client->memory_client_socket, logger_aux);
    t_segments_table *tabla_actualizada = get_tsegmento(p);
    pcb->segments_table = tabla_actualizada;

    log_info(logger_main,"PID: %d - Eliminar Segmento - Id: %d", pcb->pid, segment_id);

    // Devolver contexto a cpu
    t_pcontexto *pcontexto = create_pcontexto_from_pcb(pcb);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux); */
}

t_recurso *find_recurso(char *recurso_solicitado)
{
    for (int i = 0; i < list_size(recursos); i++)
        if (string_equals_ignore_case(recurso_solicitado, ((t_recurso *)list_get(recursos, i))->recurso))
            return list_get(recursos, i);
    return NULL;
}

int find_pcb_index(t_list *cola, uint32_t pid)
{
    for (int i = 0; i < list_size(cola); i++)
        if (((t_pcb *)list_get(cola, i))->pid == pid)
            return i;
    return -1;
}

bool execute_wait(char *recurso_solicitado, t_pcb *pcb)
{
    // busco la posicion del recurso
    t_recurso *recurso = find_recurso(recurso_solicitado);
    if (recurso == NULL)
    {
        log_info(logger_main, "El recurso %s no existe", recurso_solicitado);
        agregar_pcb_a_cola(pcb, mutex_exit, queues->EXIT);
        return false;
    }

    recurso->instancias--;
    if (recurso->instancias >= 0)
    {
        log_info(logger_main, "PID: %d - Wait: %s - Instancias: %d", pcb->pid, recurso->recurso, recurso->instancias);
        return true;
    }
    else
    {
        agregar_pcb_a_cola(pcb, mutex_blocked, queues->BLOCK);
        list_add(recurso->lista_bloqueados, pcb);
        log_info(logger_main, "PID: %d - Bloqueado por: %s", pcb->pid, recurso->recurso);
        return false;
    }
    return false;
}

bool execute_signal(char *recurso_solicitado, t_pcb *pcb)
{
    t_recurso *recurso = find_recurso(recurso_solicitado);
    if (recurso == NULL)
    {
        log_info(logger_main, "El recurso %s no existe", recurso_solicitado);
        agregar_pcb_a_cola(pcb, mutex_exit, queues->EXIT);
        return false;
    }

    recurso->instancias++;
    log_info(logger_main, "PID: %d - Signal: %s - Instancias: %d", pcb->pid, recurso->recurso, recurso->instancias);
    if (recurso->instancias <= 0)
    {
        t_pcb *pendiente = quitar_pcb_de_cola(recurso->mutex, recurso->lista_bloqueados);
        int index_pendiente = find_pcb_index(queues->BLOCK, pendiente->pid);
        quitar_pcb_de_cola_by_index(mutex_blocked, queues->BLOCK, index_pendiente);
        agregar_pcb_a_cola(pcb, mutex_ready, queues->READY);
    }
    // MANDAR A EJECUTAR DEVUELTA CPU
    return true;
}

void execute_io(int tiempo, t_pcb *pcb)
{
    agregar_pcb_a_cola(pcb, mutex_blocked, queues->BLOCK);
    log_info(logger_main, "PID: %d - Ejecuta IO: %d", pcb->pid, tiempo);
    t_io_pcb *arg = malloc(sizeof(t_io_pcb));
    arg->pid = pcb->pid;
    arg->t_sleep = tiempo;
    pthread_create(&thr_io, 0, io, (void *)arg);
}

void *io(void *args)
{
    t_io_pcb *io_pcb = (t_io_pcb *)args;
    sleep(io_pcb->t_sleep);
    int index = find_pcb_index(queues->BLOCK, io_pcb->pid);
    t_pcb *pcb = quitar_pcb_de_cola_by_index(mutex_blocked, queues->BLOCK, index);
    agregar_pcb_a_cola(pcb, mutex_ready, queues->READY);
    free(io_pcb);
    pthread_exit(0);
}

void execute_exit(t_pcb *pcb, char *motivo)
{
    log_info(logger_main, "Finaliza el proceso %d - Motivo: %s", pcb->pid, motivo);
    agregar_pcb_a_cola(pcb, mutex_exit, queues->EXIT);
}

void execute_to_ready(t_pcb *pcb)
{
    pcb->tiempo_llegada_ready = temporal_create();
    agregar_pcb_a_cola(pcb, mutex_ready, queues->READY);
}