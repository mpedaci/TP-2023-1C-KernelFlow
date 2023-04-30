#include "kernel_core.h"

void start_kernel_core()
{
    pthread_create(&thr_core, 0, process_queues, NULL);
}

void end_kernel_core()
{
    core_running = false;
    pthread_join(thr_core, NULL);
}

// CONTROLER

void *process_queues()
{
    t_pcb *to_execute = NULL;
    while (core_running)
    {
        // NEW -> READY (FIFO)
        if (can_move_NEW_to_READY())
        {
            t_pcb *pcb = queue_pop(queues->NEW);
            queue_push(queues->READY, pcb);
        }
        // READY -> EXEC
        if (algorithm_is_FIFO() && can_execute_process())
            to_execute = FIFO(queues->READY);
        else if (algorithm_is_HRRN() && can_execute_process())
            to_execute = HRRN(queues->READY);
        if (to_execute != NULL)
        {
            queue_push(queues->EXEC, to_execute);
            to_execute = NULL;
            execute_process();
        }
        // EXEC -> BLOCK

        // EXEC -> READY

        // EXEC -> EXIT

        // BLOCK -> READY

        // FINALLY
        sleep(1);
    }
    // free(to_execute); // NO HARIA FALTA SI ESTA DENTRO DE UNA COLA
    pthread_exit(0);
}

// MOVIMIENTO DE COLAS

void queue_add(t_list *destiny, t_pcb *pcb)
{
    list_add(destiny, pcb);
}

void queue_remove(t_list *origin, t_pcb *pcb)
{
    list_remove_element(origin, pcb);
}

t_pcb *queue_pop(t_list *origin)
{
    return list_remove(origin, 0);
}

void queue_push(t_list *destiny, t_pcb *pcb)
{
    list_add(destiny, pcb);
}

bool queue_is_empty(t_list *queue)
{
    return list_is_empty(queue);
}

// CONDICIONES

bool can_move_NEW_to_READY()
{
    return list_size(queues->NEW) > 0 && list_size(queues->READY) < config_kernel->grado_max_multiprog;
}

bool algorithm_is_FIFO()
{
    return string_equals_ignore_case(config_kernel->algoritmo_planificacion, "FIFO");
}

bool algorithm_is_HRRN()
{
    return string_equals_ignore_case(config_kernel->algoritmo_planificacion, "HRRN");
}

bool can_execute_process()
{
    return !queue_is_empty(queues->READY) && queue_is_empty(queues->EXEC);
}

// PCB

t_pcontexto *create_pcontexto_from_pcb(t_pcb *pcb)
{
    // ACA TALVEZ DEBA COPIAR LA MEMORIA
    t_pcontexto *pcontexto = malloc(sizeof(t_pcontexto));
    pcontexto->pid = pcb->pid;
    pcontexto->instructions = pcb->instrucciones;
    pcontexto->program_counter = pcb->program_counter;
    pcontexto->registers = pcb->registers;
    return pcontexto;
}

void update_pcb_from_pcontexto(t_pcb *pcb, t_pcontexto *pcontexto)
{
    pcb->program_counter = pcontexto->program_counter;
    pcb->registers = pcontexto->registers;
}

// ALGORITMOS
void execute_process()
{
    /* t_pcb *pcb = queue_pop(queues->EXEC);
    t_pcontexto *pcontexto = create_pcontexto_from_pcb(pcb);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux);
    t_package *p = get_package(modules_client->cpu_client_socket, logger_aux);
    t_pcontexto *pcontexto_response = get_pcontexto(p);
    update_pcb_from_pcontexto(pcb, pcontexto_response);
    // CHEQUEAR ULTIMA INST
    free(pcontexto);
    free(pcontexto_response); */
}


