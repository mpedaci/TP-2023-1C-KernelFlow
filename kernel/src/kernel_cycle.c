#include "kernel_cycle.h"

// NEW -> READY

bool request_t_segment(t_pcb *pcb)
{
    bool created = false;
    t_pid_status *pid_status = pid_status_create(pcb->pid, PROCESS_NEW);
    send_pid_status(modules_client->memory_client_socket, pid_status, logger_aux);
    pid_status_destroy(pid_status);
    log_info(logger_aux, "PID: %d | Esperando tabla de segmentos de memoria", pcb->pid);
    t_package *package = get_package(modules_client->memory_client_socket, logger_aux);
    switch (package->operation_code)
    {
    case TSEGMENTOS:
        t_segments_table *ts = get_tsegmento(package);
        segments_table_destroy(pcb->segments_table);
        pcb->segments_table = ts;
        created = true;
        break;
    default:
        log_error(logger_aux, "PID: %d | Error al recibir tabla de segmentos de memoria", pcb->pid);
        break;
    }
    package_destroy(package);
    return created;
}

// EXEC -> READY | BLOCK | EXIT

void EXEC(t_pcb *pcb, bool is_second_execution)
{
    if (pcb == NULL)
        pcb = get_pcb_from(QEXEC);
    if (pcb != NULL)
    {
        log_info(logger_aux, "PID: %d | Ejecutando", pcb->pid);
        // 1. Enviar PContexto a CPU
        log_info(logger_aux, "PID: %d | Enviando contexto a CPU", pcb->pid);
        send_pcb_to_cpu(pcb);
        if (!is_second_execution)
        {
            temporal_destroy(pcb->tiempo_entrada_cpu);
            pcb->tiempo_entrada_cpu = temporal_create();
        }
        // 2. Recibir PContexto de CPU
        log_info(logger_aux, "PID: %d | Esperando respuesta de CPU", pcb->pid);
        t_pcontexto_desalojo *pcontexto_response = receive_pcontext_from_cpu();
        if (pcontexto_response != NULL)
        {
            // 3. Actualizar PCB
            update_pcb(pcb, pcontexto_response);
            // 4. Analizar motivo de desalojo
            process_pcontexto_desalojo(pcb, pcontexto_response);
            // 5. Mover PCB a cola correspondiente
            if (!is_second_execution)
            {
                temporal_destroy(pcb->tiempo_salida_cpu);
                pcb->tiempo_salida_cpu = temporal_create();
                move_pcb_from_to(pcb, QEXEC, pcb->next_queue);
                if (pcb->next_queue == QEXIT)
                    EXIT(pcb);
            }
            // 6. Eliminar PContexto
            pcontexto_desalojo_destroy(pcontexto_response);
        }
        else
        {
            log_error(logger_aux, "PID: %d | Error al recibir respuesta de CPU", pcb->pid);
            pcb->exit_status = ERROR;
            pcb->next_queue = QEXIT;
            move_pcb_from_to(pcb, QEXEC, QEXIT);
            EXIT(pcb);
        }
    }
}

void send_pcb_to_cpu(t_pcb *pcb)
{
    t_pcontexto *pcontexto = pcontexto_create(pcb);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux);
    pcontexto_destroy(pcontexto);
}

t_pcontexto_desalojo *receive_pcontext_from_cpu()
{
    t_pcontexto_desalojo *pcontexto_response = NULL;
    t_package *package = get_package(modules_client->cpu_client_socket, logger_aux);
    switch (package->operation_code)
    {
    case PCONTEXTODESALOJO:
        pcontexto_response = get_pcontexto_desalojo(package);
        break;
    default:
        break;
    }
    package_destroy(package);
    return pcontexto_response;
}

void update_pcb(t_pcb *pcb, t_pcontexto_desalojo *pcontexto)
{
    pcb->program_counter = pcontexto->program_counter;
    registers_duplicate(pcontexto->registers, pcb->registers);
}

void process_pcontexto_desalojo(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_response)
{
    bool se = false;
    if (pcontexto_response->status_code == SUCCESS)
    {
        switch (pcontexto_response->motivo_desalojo->identificador)
        {
        case I_WAIT:
            se = execute_wait(pcb, pcontexto_response);
            break;
        case I_SIGNAL:
            se = execute_signal(pcb, pcontexto_response);
            break;
        case I_I_O:
            se = execute_io(pcb, pcontexto_response);
            break;
        case I_YIELD:
            se = execute_yield(pcb, pcontexto_response);
            break;
        case I_EXIT:
            se = execute_exit(pcb, pcontexto_response);
            break;
        case I_F_WRITE:
            se = execute_fwrite(pcb, pcontexto_response);
            break;
        case I_F_CLOSE:
            se = execute_fclose(pcb, pcontexto_response);
            break;
        case I_F_OPEN:
            se = execute_fopen(pcb, pcontexto_response);
            break;
        case I_F_READ:
            se = execute_fread(pcb, pcontexto_response);
            break;
        case I_F_SEEK:
            se = execute_fseek(pcb, pcontexto_response);
            break;
        case I_F_TRUNCATE:
            se = execute_ftruncate(pcb, pcontexto_response);
            break;
        case I_CREATE_SEGMENT:
            se = execute_create_segment(pcb, pcontexto_response);
            break;
        case I_DELETE_SEGMENT:
            se = execute_delete_segment(pcb, pcontexto_response);
            break;
        default:
            if (pcb->exit_status != SUCCESS || pcontexto_response->status_code != SUCCESS)
                se = execute_exit(pcb, pcontexto_response);
            break;
        }
        if (se)
            EXEC(pcb, true);
    } else {
        se = execute_exit(pcb, pcontexto_response);
    }
}

// EXIT

void EXIT(t_pcb *pcb)
{
    char *status_code = status_code_string(pcb->exit_status);
    log_info(logger_main, "Finaliza el proceso %d - Motivo: %s", pcb->pid, status_code);
    t_pid_status *pid_status = pid_status_create(pcb->pid, PROCESS_END);
    send_pid_status(modules_client->memory_client_socket, pid_status, logger_aux);
    pid_status_destroy(pid_status);

    // Libero recursos
    // 1. Archivos abiertos
    if (list_size(pcb->open_files_table) > 0)
    {
        for (int i = 0; i < list_size(pcb->open_files_table); i++)
        {
            t_archivo_abierto *archivo_abierto = list_get(pcb->open_files_table, i);
            archivo_abierto->archivo->instancias++;
            log_info(logger_main, "PID: %d - Cerrar Archivo: %s", pcb->pid, archivo_abierto->archivo->recurso);
            if (archivo_abierto->archivo->instancias > 0)
            {
                list_remove_element(archivos_abiertos, archivo_abierto->archivo);
                recurso_destroy(archivo_abierto->archivo);
            }
            else
            {
                t_pcb *pendiente = remove_pcb_from_queue_resourse(archivo_abierto->archivo);
                move_pcb_from_to(pendiente, QBLOCK, QREADY);
            }
            free(archivo_abierto);
        }
        list_destroy(pcb->open_files_table);
        pcb->open_files_table = list_create();
    }
    // 2. Recursos compartidos
    if (list_size(pcb->shared_resources) > 0)
    {
        for (int i = 0; i < list_size(pcb->shared_resources); i++)
        {
            t_recurso *recurso = list_get(pcb->shared_resources, i);
            recurso->instancias++;
            log_info(logger_main, "PID: %d - Liberar Recurso: %s", pcb->pid, recurso->recurso);
            bool is_blocked = false;
            for (int j = 0; j < list_size(recurso->lista_bloqueados); j++)
            {
                t_pcb *pcb_blocked = list_get(recurso->lista_bloqueados, j);
                if (pcb_blocked->pid == pcb->pid)
                {
                    is_blocked = true;
                    break;
                }
            }
            if (is_blocked)
                list_remove_element(recurso->lista_bloqueados, pcb);
            else
            {
                if (recurso->instancias <= 0)
                {
                    t_pcb *pendiente = remove_pcb_from_queue_resourse(recurso);
                    move_pcb_from_to(pendiente, QBLOCK, QREADY);
                }
            }
        }
        list_destroy(pcb->shared_resources);
        pcb->shared_resources = list_create();
    }
}
