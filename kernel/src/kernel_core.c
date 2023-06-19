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
            t_pcb *pcb = pop_pcb_from_queue(QNEW);
            pcb->tiempo_llegada_ready = temporal_create();
            if (request_t_segment(pcb))
                add_pcb_to_queue(QREADY, pcb);
            else
                add_pcb_to_queue(QEXIT, pcb);
        }
        // READY -> EXEC
        if (algorithm_is_FIFO() && can_execute_process())
            to_execute = FIFO(queues->READY);
        else if (algorithm_is_HRRN() && can_execute_process())
            to_execute = HRRN(queues->READY);
        if (to_execute != NULL)
        {
            add_pcb_to_queue(QEXEC, to_execute);
            to_execute = NULL;
            execute();
        }
        // EXEC -> BLOCK - OK
        // EXEC -> READY - OK
        // EXEC -> EXIT - OK
        // BLOCK -> READY - OK
        sleep(1);
    }
    log_info(logger_aux, "Thread Process Queues: finalizado");
    pthread_exit(0);
}

// CONDICIONES

bool can_move_NEW_to_READY()
{
    int actual_multiprog = list_size(queues->READY) + list_size(queues->EXEC) + list_size(queues->BLOCK);
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
    return !is_queue_empty(QREADY) && is_queue_empty(QEXEC);
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

void destroy_mutex()
{
    pthread_mutex_destroy(&mutex_new);
    pthread_mutex_destroy(&mutex_ready);
    pthread_mutex_destroy(&mutex_running);
    pthread_mutex_destroy(&mutex_blocked);
    pthread_mutex_destroy(&mutex_exit);
    pthread_mutex_destroy(&mutex_pid);
}

// PCB

bool request_t_segment(t_pcb *pcb)
{
    bool created = false;
    t_pid_status *pid_status = malloc(sizeof(t_pid_status));
    pid_status->pid = pcb->pid;
    pid_status->status = PROCESS_NEW;
    send_pid_status(modules_client->memory_client_socket, pid_status, logger_aux);
    t_package *package = get_package(modules_client->memory_client_socket, logger_aux);
    switch (package->operation_code)
    {
    case TSEGMENTOS:
        t_segments_table *ts = get_tsegmento(package);
        pcb->segments_table = ts;
        created = true;
        break;
    default:
        log_error(logger_aux, "Error al recibir tabla de segmentos");
        break;
    }
    package_destroy(package);
    free(pid_status);
    return created;
}

t_pcontexto *create_pcontexto_from_pcb(t_pcb *pcb)
{
    t_pcontexto *pcontexto = malloc(sizeof(t_pcontexto));
    pcontexto->pid = pcb->pid;
    pcontexto->program_counter = pcb->program_counter;
    pcontexto->registers = init_registers();
    copy_registers(pcontexto->registers, pcb->registers);
    pcontexto->instructions = copy_instructions_list(pcb->instrucciones);
    pcontexto->segments = copy_segment_list(pcb->segments_table);
    return pcontexto;
}

void update_pcb_from_pcontexto(t_pcb *pcb, t_pcontexto_desalojo *pcontexto)
{
    pcb->program_counter = pcontexto->program_counter;
    copy_registers(pcb->registers, pcontexto->registers);
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

    free_pcontexto(pcontexto);
    free_pcontexto_desalojo(pcontexto_response);
    package_destroy(p);
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

    free_pcontexto(pcontexto);
    free_pcontexto_desalojo(pcontexto_response);
    package_destroy(p);
}

void procesar_motivo_desalojo(t_pcontexto_desalojo *pcontexto_response)
{
    char *recurso_solicitado;
    bool se = false;
    t_pcb *pcb = pop_pcb_from_queue(QEXEC);
    switch (pcontexto_response->motivo_desalojo->identificador)
    {
    case I_WAIT:
        recurso_solicitado = list_get(pcontexto_response->motivo_desalojo->parametros, 0);
        se = execute_wait(recurso_solicitado, pcb);
        if (se)
        {
            add_pcb_to_queue(QEXEC, pcb);
            sexecute();
        }
        // free(recurso_solicitado);
        break;
    case I_SIGNAL:
        recurso_solicitado = list_get(pcontexto_response->motivo_desalojo->parametros, 0);
        se = execute_signal(recurso_solicitado, pcb);
        if (se)
        {
            add_pcb_to_queue(QEXEC, pcb);
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
        execute_exit(pcb, SUCCESS);
        break;
    case I_F_WRITE:
        execute_fwrite(pcontexto_response->motivo_desalojo, pcb);
        break;
    case I_F_CLOSE:
        execute_exit(pcb, SUCCESS);
        break;
    case I_F_OPEN:
        execute_exit(pcb, SUCCESS);
        break;
    case I_F_READ:
        execute_fread(pcontexto_response->motivo_desalojo, pcb);
        break;
    case I_F_SEEK:
        execute_exit(pcb, SUCCESS);
        break;
    case I_F_TRUNCATE:
        execute_ftruncate(pcontexto_response->motivo_desalojo, pcb);
        break;
    case I_CREATE_SEGMENT:
        se = execute_create_segment(pcontexto_response->motivo_desalojo, pcb);
        if (se)
        {
            add_pcb_to_queue(QEXEC, pcb);
            sexecute();
        }
        break;
    case I_DELETE_SEGMENT:
        se = execute_delete_segment(pcontexto_response->motivo_desalojo, pcb);
        if (se)
        {
            add_pcb_to_queue(QEXEC, pcb);
            sexecute();
        }
        break;
    default:
        break;
    }
}
