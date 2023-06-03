#include "kernel_core.h"

void start_kernel_core()
{
    pthread_create(&thr_core, 0, process_queues, NULL);
}

void end_kernel_core()
{
    core_running = false;
    pthread_join(thr_core, NULL);
    log_info(logger_aux, "Thread Kernel Core: finalizado");
}

// CONTROLER

void *process_queues()
{
    t_pcb *to_execute = NULL;
    while (core_running)
    {
        // NEW -> READY (FIFO)
        while (can_move_NEW_to_READY())
        {
            t_pcb *pcb = queue_pop(queues->NEW);
            pcb->tiempo_llegada_ready = temporal_create();
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
            execute();
        }
        // EXEC -> BLOCK

        // EXEC -> READY

        // EXEC -> EXIT

        // BLOCK -> READY

        // FINALLY
        sleep(1);
    }
    // free(to_execute); // NO HARIA FALTA SI ESTA DENTRO DE UNA COLA
    log_info(logger_aux, "Thread Process Queues: finalizado");
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
    int actual_multiprog = list_size(queues->READY) + list_size(queues->EXEC) + list_size(queues->BLOCK);
    // log_debug(logger_aux, "Actual multiprog: %d", actual_multiprog);
    return list_size(queues->NEW) > 0 && actual_multiprog < config_kernel->grado_max_multiprog;
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

// Sincronizacion de colas y listas (VER)

void agregar_pcb_a_cola(t_pcb *pcb, pthread_mutex_t mutex, t_list *cola)
{
    pthread_mutex_lock(&mutex);
    queue_push(cola, pcb);
    pthread_mutex_unlock(&mutex);
}

t_pcb *quitar_pcb_de_cola(pthread_mutex_t mutex, t_list *cola)
{
    pthread_mutex_lock(&mutex);
    t_pcb *pcb = queue_pop(cola);
    pthread_mutex_unlock(&mutex);
    return pcb;
}


t_pcb *quitar_pcb_de_cola_by_index(pthread_mutex_t mutex, t_list *cola, int index)
{
    pthread_mutex_lock(&mutex);
    t_pcb *pcb = list_remove(cola, index);
    pthread_mutex_unlock(&mutex);
    return pcb;
}

void agregar_pcb_a_lista(t_pcb *pcb, pthread_mutex_t mutex, t_list *cola)
{
    pthread_mutex_lock(&mutex);
    list_add(cola, pcb);
    pthread_mutex_unlock(&mutex);
}

t_pcb *quitar_primer_pcb_de_lista(pthread_mutex_t mutex, t_list *cola)
{
    pthread_mutex_lock(&mutex);
    t_pcb *pcb = list_remove(cola, 0);
    pthread_mutex_unlock(&mutex);
    return pcb;
}

void inicializar_mutex()
{
    pthread_mutex_init(&mutex_new, NULL);
    pthread_mutex_init(&mutex_ready, NULL);
    pthread_mutex_init(&mutex_running, NULL);
    pthread_mutex_init(&mutex_blocked, NULL);
    pthread_mutex_init(&mutex_exit, NULL);
    pthread_mutex_init(&mutex_pid, NULL);
}

// PCB

t_pcontexto *create_pcontexto_from_pcb(t_pcb *pcb)
{
    // ACA TALVEZ DEBA COPIAR LA MEMORIA
    t_pcontexto *pcontexto = malloc(sizeof(t_pcontexto));
    pcontexto->pid = pcb->pid;
    pcontexto->program_counter = pcb->program_counter;
    pcontexto->registers = pcb->registers;
    pcontexto->instructions = pcb->instrucciones;
    return pcontexto;
}

void update_pcb_from_pcontexto(t_pcb *pcb, t_pcontexto_desalojo *pcontexto)
{
    pcb->program_counter = pcontexto->program_counter;
    pcb->registers = pcontexto->registers;
}

void cargar_recursos()
{
    recursos = list_create();
    for (int i = 0; i < list_size(config_kernel->recursos); i++)
    {
        t_recurso *recurso = malloc(sizeof(t_recurso));
        recurso->recurso = list_get(config_kernel->recursos, i);
        recurso->instancias = atoi((char *)list_get(config_kernel->instancias_recursos, i));
        recurso->lista_bloqueados = list_create(); // PCBs Bloqueados
        list_add(recursos, recurso);
        pthread_mutex_init(&recurso->mutex, NULL);
    }
}

void execute()
{
    t_pcb *pcb = list_get(queues->EXEC, 0);
    log_info(logger_aux, "PID: %d | Ejecutando", pcb->pid);
    pcb->tiempo_entrada_cpu = temporal_create();
    t_pcontexto *pcontexto = create_pcontexto_from_pcb(pcb);
    log_info(logger_aux, "PID: %d | Enviando contexto a CPU", pcontexto->pid);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux);

    log_info(logger_aux, "PID: %d | Esperando respuesta de CPU", pcontexto->pid);
    t_package *p = get_package(modules_client->cpu_client_socket, logger_aux);
    t_pcontexto_desalojo *pcontexto_response = get_pcontexto_desalojo(p);
    update_pcb_from_pcontexto(pcb, pcontexto_response);

    log_info(logger_aux, "PID: %d | Procesando motivo de desalojo: %d", pcontexto->pid, pcontexto_response->motivo_desalojo->identificador);
    procesar_motivo_desalojo(pcontexto_response);

    pcb->tiempo_salida_cpu = temporal_create();
    update_est_sig_rafaga(pcb);

    free(pcontexto);
    free_pcontexto_desalojo(pcontexto_response);
}

void sexecute()
{
    t_pcb *pcb = list_get(queues->EXEC, 0);
    t_pcontexto *pcontexto = create_pcontexto_from_pcb(pcb);
    log_info(logger_aux, "PID: %d | Enviando contexto a CPU", pcontexto->pid);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux);

    log_info(logger_aux, "PID: %d | Esperando respuesta de CPU", pcontexto->pid);
    t_package *p = get_package(modules_client->cpu_client_socket, logger_aux);
    t_pcontexto_desalojo *pcontexto_response = get_pcontexto_desalojo(p);
    update_pcb_from_pcontexto(pcb, pcontexto_response);

    log_info(logger_aux, "PID: %d | Procesando motivo de desalojo: %d", pcontexto->pid, pcontexto_response->motivo_desalojo->identificador);
    procesar_motivo_desalojo(pcontexto_response);

    free(pcontexto);
    free_pcontexto_desalojo(pcontexto_response);
}

void procesar_motivo_desalojo(t_pcontexto_desalojo *pcontexto_response)
{
    char *recurso_solicitado;
    bool se = false;
    t_pcb *pcb = quitar_primer_pcb_de_lista(mutex_running, queues->EXEC);
    switch (pcontexto_response->motivo_desalojo->identificador)
    {
    case I_WAIT:
        recurso_solicitado = list_get(pcontexto_response->motivo_desalojo->parametros, 0);
        se = execute_wait(recurso_solicitado, pcb);
        if (se) {
            queue_add(queues->EXEC, pcb);
            sexecute();
        }
        // free(recurso_solicitado);
        break;
    case I_SIGNAL:
        recurso_solicitado = list_get(pcontexto_response->motivo_desalojo->parametros, 0);
        se = execute_signal(recurso_solicitado, pcb);
        if (se) {
            queue_add(queues->EXEC, pcb);
            sexecute();
        }
        // free(recurso_solicitado);
        break;
    case I_I_O:
        int tiempo = atoi(list_get(pcontexto_response->motivo_desalojo->parametros, 0));
        execute_io(tiempo, pcb);
        break;
    case I_YIELD:
        execute_to_ready(pcb);
        break;
    case I_EXIT:
        execute_exit(pcb, "SUCCESS");
        break;
    case I_F_WRITE:
        execute_exit(pcb, "NO IMPLEMENTADO");
        break;
    case I_F_CLOSE:
        execute_exit(pcb, "NO IMPLEMENTADO");
        break;
    case I_F_OPEN:
        execute_exit(pcb, "NO IMPLEMENTADO");
        break;
    case I_F_READ:
        execute_exit(pcb, "NO IMPLEMENTADO");
        break;
    case I_F_SEEK:
        execute_exit(pcb, "NO IMPLEMENTADO");
        break;
    case I_F_TRUNCATE:
        execute_exit(pcb, "NO IMPLEMENTADO");
        break;
    case I_CREATE_SEGMENT:
        // uint32_t tamanio_solicitado = atoi(list_get(instruccion_desalojo->parametros, 1));
        // uint32_t id_solicitado = atoi(list_get(instruccion_desalojo->parametros, 2));
        // execute_create_segment(tamanio_solicitado, id_solicitado, pcb);
        execute_exit(pcb, "NO IMPLEMENTADO");
        break;
    case I_DELETE_SEGMENT:
        // uint32_t id = atoi(list_get(instruccion_desalojo->parametros, 0));
        // execute_delete_segment(id, pcb);
        execute_exit(pcb, "NO IMPLEMENTADO");
        break;
    default:
        break;
    }
}