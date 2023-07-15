#include "kernel_instructions.h"

// EXECUTE RECURSOS

bool execute_wait(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    char *recurso_solicitado = list_get(pcontexto_desalojo->motivo_desalojo->parametros, 0);
    t_recurso *recurso = find_recurso(recurso_solicitado);
    if (recurso == NULL)
    {
        log_info(logger_main, "PID: %d - Wait: %s - No existe el recurso", pcb->pid, recurso_solicitado);
        pcb->next_queue = QEXIT;
        pcb->exit_status = INVALID_RESOURCE;
        return false;
    }
    recurso->instancias--;
    list_add(pcb->shared_resources, recurso);
    if (recurso->instancias >= 0)
    {
        log_info(logger_main, "PID: %d - Wait: %s - Instancias: %d", pcb->pid, recurso->recurso, recurso->instancias);
        return true;
    }
    else
    {
        pcb->next_queue = QBLOCK;
        list_add(recurso->lista_bloqueados, pcb);
        log_info(logger_main, "PID: %d - Bloqueado por: %s", pcb->pid, recurso->recurso);
        return false;
    }
}

bool execute_signal(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    char *recurso_solicitado = list_get(pcontexto_desalojo->motivo_desalojo->parametros, 0);
    t_recurso *recurso = find_recurso(recurso_solicitado);
    if (recurso == NULL)
    {
        log_info(logger_main, "PID: %d - Signal: %s - No existe el recurso", pcb->pid, recurso_solicitado);
        pcb->next_queue = QEXIT;
        pcb->exit_status = INVALID_RESOURCE;
        return false;
    }
    recurso->instancias++;
    list_remove_element(pcb->shared_resources, recurso);
    log_info(logger_main, "PID: %d - Signal: %s - Instancias: %d", pcb->pid, recurso->recurso, recurso->instancias);
    if (recurso->instancias <= 0)
    {
        t_pcb *pendiente = remove_pcb_from_queue_resourse(recurso);
        move_pcb_from_to(pendiente, QBLOCK, QREADY);
    }
    return true;
}

bool execute_io(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    int tiempo = atoi(list_get(pcontexto_desalojo->motivo_desalojo->parametros, 0));
    pcb->next_queue = QBLOCK;
    log_info(logger_main, "PID: %d - Ejecuta IO: %d", pcb->pid, tiempo);
    t_io_pcb *arg = malloc(sizeof(t_io_pcb));
    arg->pcb = pcb;
    arg->t_sleep = tiempo;
    pthread_create(&thr_io, 0, io, (void *)arg);
    return false;
}

void *io(void *args)
{
    t_io_pcb *io_pcb = (t_io_pcb *)args;
    sleep(io_pcb->t_sleep);
    move_pcb_from_to(io_pcb->pcb, QBLOCK, QREADY);
    free(io_pcb);
    pthread_exit(0);
}

// EXECUTE KERNEL

bool execute_yield(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    pcb->next_queue = QREADY;
    return false;
}

bool execute_exit(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    pcb->exit_status = pcontexto_desalojo->status_code;
    pcb->next_queue = QEXIT;
    return false;
}

// EXECUTE ARCHIVOS

bool execute_fwrite(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    t_instruccion *instruccion = instruccion_duplicate(pcontexto_desalojo->motivo_desalojo);
    char *archivo = list_get(instruccion->parametros, 0);
    t_archivo_abierto *archivo_abierto = buscar_archivo_abierto(pcb, archivo);
    // Agrego informacion a la instruccion
    char puntero[16];
    sprintf(puntero, "%u", archivo_abierto->puntero);
    list_add(instruccion->parametros, puntero);
    instruccion->p_length[3] = strlen(puntero) + 1;
    instruccion->cant_parametros++;
    // Armo la tarea para el hilo
    t_pcb_inst_status *arg = malloc(sizeof(t_pcb_inst_status));
    arg->pcb = pcb;
    arg->instruccion = instruccion;
    arg->status_expected = FILE_WRITTEN;
    // Genero el hilo de trabajo
    pthread_create(&thr_file, 0, file_processing, (void *)arg);
    log_info(logger_main,
             "PID: %d - Escribir Archivo: %s - Puntero: %d - Direccion Memoria: %d - Tamanio: %d",
             pcb->pid, archivo, archivo_abierto->puntero, atoi(list_get(instruccion->parametros, 1)), atoi(list_get(instruccion->parametros, 2)));

    pcb->next_queue = QBLOCK;
    log_info(logger_main, "PID: %d - Bloqueado por: %s", pcb->pid, archivo);
    return false;
}

bool execute_fread(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    t_instruccion *instruccion = instruccion_duplicate(pcontexto_desalojo->motivo_desalojo);
    char *archivo = list_get(instruccion->parametros, 0);
    t_archivo_abierto *archivo_abierto = buscar_archivo_abierto(pcb, archivo);
    // Agrego informacion a la instruccion
    char puntero[16];
    sprintf(puntero, "%u", archivo_abierto->puntero);
    list_add(instruccion->parametros, puntero);
    instruccion->p_length[3] = strlen(puntero) + 1;
    instruccion->cant_parametros++;
    // Armo la tarea para el hilo
    t_pcb_inst_status *arg = malloc(sizeof(t_pcb_inst_status));
    arg->pcb = pcb;
    arg->instruccion = instruccion;
    arg->status_expected = FILE_READ;
    // Genero el hilo de trabajo
    pthread_create(&thr_file, 0, file_processing, (void *)arg);
    log_info(logger_main,
             "PID: %d - Leer Archivo: %s - Puntero: %d - Direccion Memoria: %d - Tamanio: %d",
             pcb->pid, archivo, archivo_abierto->puntero, atoi(list_get(instruccion->parametros, 1)), atoi(list_get(instruccion->parametros, 2)));

    pcb->next_queue = QBLOCK;
    log_info(logger_main, "PID: %d - Bloqueado por: %s", pcb->pid, archivo);
    return false;
}

bool execute_fcreate(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    t_instruccion *instruccion = pcontexto_desalojo->motivo_desalojo;
    bool status = false;
    char *nombre_archivo = list_get(instruccion->parametros, 0);
    t_instruccion *instfcreate = instruccion_duplicate(instruccion);
    char *c = "create";
    list_add(instfcreate->parametros, c);
    instfcreate->cant_parametros++;
    instfcreate->p_length[1] = strlen(c) + 1;
    pthread_mutex_lock(&mutex_fs_connection);
    send_instruccion(modules_client->filesystem_client_socket, instfcreate, logger_aux);
    t_package *package = get_package(modules_client->filesystem_client_socket, logger_aux);
    pthread_mutex_unlock(&mutex_fs_connection);
    list_remove(instfcreate->parametros, 1);
    instfcreate->cant_parametros--;
    instfcreate->p_length[1] = 0;
    instruccion_destroy(instfcreate);
    switch (package->operation_code)
    {
    case STATUS_CODE:
        t_status_code code = get_status_code(package);
        if (code == FILE_OPEN || code == FILE_CREATED)
        {
            log_info(logger_main, "PID: %d - Crear Archivo: %s", pcb->pid, nombre_archivo);
            t_recurso *nuevo_archivo = crear_archivo(nombre_archivo);
            list_add(archivos_abiertos, nuevo_archivo);
            t_archivo_abierto *archivo_abierto = archivo_abierto_create(nuevo_archivo);
            list_add(pcb->open_files_table, archivo_abierto);
            nuevo_archivo->instancias--;
            status = true;
        }
        else
        {
            log_error(logger_main, "PID: %d - Crear Archivo: %s - Error al crear archivo", pcb->pid, nombre_archivo);
            pcb->exit_status = code;
            pcb->next_queue = QEXIT;
        }
        break;
    default:
        log_error(logger_main, "PID: %d - Abrir Archivo: %s - Error al recibir respuesta de FS", pcb->pid, nombre_archivo);
        pcb->exit_status = ERROR;
        pcb->next_queue = QEXIT;
        break;
    }
    package_destroy(package);
    return status;
}

bool execute_fopen(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    t_instruccion *instruccion = pcontexto_desalojo->motivo_desalojo;
    bool status = false;
    char *nombre_archivo = list_get(instruccion->parametros, 0);
    t_recurso *archivo = buscar_archivo(nombre_archivo);
    if (archivo == NULL)
    {
        pthread_mutex_lock(&mutex_fs_connection);
        send_instruccion(modules_client->filesystem_client_socket, instruccion, logger_aux);
        t_package *package = get_package(modules_client->filesystem_client_socket, logger_aux);
        pthread_mutex_unlock(&mutex_fs_connection);
        switch (package->operation_code)
        {
        case STATUS_CODE:
            t_status_code code = get_status_code(package);
            if (code == FILE_OPEN)
            {
                log_info(logger_main, "PID: %d - Abrir Archivo: %s", pcb->pid, nombre_archivo);
                t_recurso *nuevo_archivo = crear_archivo(nombre_archivo);
                list_add(archivos_abiertos, nuevo_archivo);
                t_archivo_abierto *archivo_abierto = archivo_abierto_create(nuevo_archivo);
                list_add(pcb->open_files_table, archivo_abierto);
                nuevo_archivo->instancias--;
                status = true;
            }
            else if (code == FILE_NOT_EXIST)
            {
                status = execute_fcreate(pcb, pcontexto_desalojo);
            }
            else
            {
                log_error(logger_main, "PID: %d - Abrir Archivo: %s - Error al abrir archivo", pcb->pid, nombre_archivo);
                pcb->exit_status = code;
                pcb->next_queue = QEXIT;
            }
            break;
        default:
            log_error(logger_main, "PID: %d - Abrir Archivo: %s - Error al recibir respuesta de FS", pcb->pid, nombre_archivo);
            pcb->exit_status = ERROR;
            pcb->next_queue = QEXIT;
            break;
        }
        package_destroy(package);
    }
    else
    {
        // Si existe es porque otro lo posee asignado
        t_archivo_abierto *archivo_abierto = archivo_abierto_create(archivo);
        list_add(pcb->open_files_table, archivo_abierto);
        list_add(archivo->lista_bloqueados, pcb);
        archivo->instancias--;
        pcb->next_queue = QBLOCK;
        log_info(logger_main, "PID: %d - Bloqueado por: %s", pcb->pid, archivo->recurso);
    }
    return status;
}

bool execute_fclose(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    t_instruccion *instruccion = pcontexto_desalojo->motivo_desalojo;
    char *nombre_archivo = list_get(instruccion->parametros, 0);
    t_recurso *archivo = buscar_archivo(nombre_archivo);
    if (archivo == NULL)
    {
        log_error(logger_main, "PID: %d - Cerrar archivo: %s - No existe el archivo", pcb->pid, nombre_archivo);
        pcb->exit_status = INVALID_RESOURCE;
        pcb->next_queue = QEXIT;
        return false;
    }
    else
    {
        t_archivo_abierto *archivo_abierto = buscar_archivo_abierto(pcb, nombre_archivo);
        if (archivo_abierto == NULL)
        {
            log_error(logger_main, "PID: %d - Cerrar archivo: %s - No esta abierto el archivo", pcb->pid, nombre_archivo);
            pcb->exit_status = INVALID_RESOURCE;
            pcb->next_queue = QEXIT;
            return false;
        }
        else
        {
            int index_archivo_abierto = find_archivo_abierto_index(pcb, nombre_archivo);
            t_archivo_abierto *archivo_abierto = list_remove(pcb->open_files_table, index_archivo_abierto);
            free(archivo_abierto);
            archivo->instancias++;
            log_info(logger_main, "PID: %d - Cerrar Archivo: %s", pcb->pid, nombre_archivo);
            if (archivo->instancias > 0)
            {
                list_remove_element(archivos_abiertos, archivo);
                recurso_destroy(archivo);
            }
            else
            {
                t_pcb *pendiente = remove_pcb_from_queue_resourse(archivo);
                move_pcb_from_to(pendiente, QBLOCK, QREADY);
            }
            return true;
        }
    }
}

bool execute_fseek(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    t_instruccion *instruccion = pcontexto_desalojo->motivo_desalojo;
    char *nombre_archivo = list_get(instruccion->parametros, 0);
    t_recurso *archivo = buscar_archivo(nombre_archivo);
    if (archivo == NULL)
    {
        log_error(logger_main, "PID: %d - Actualizar puntero Archivo: %s - No existe el archivo", pcb->pid, nombre_archivo);
        pcb->exit_status = INVALID_RESOURCE;
        pcb->next_queue = QEXIT;
        return false;
    }
    else
    {
        t_archivo_abierto *archivo_abierto = buscar_archivo_abierto(pcb, nombre_archivo);
        if (archivo_abierto == NULL)
        {
            log_error(logger_main, "PID: %d - Actualizar puntero Archivo: %s - No esta abierto el archivo", pcb->pid, nombre_archivo);
            pcb->exit_status = INVALID_RESOURCE;
            pcb->next_queue = QEXIT;
            return false;
        }
        else
        {
            archivo_abierto->puntero = atoi(list_get(instruccion->parametros, 1));
            log_info(logger_main, "PID: %d - Actualizar puntero Archivo: %s - Puntero %d", pcb->pid, nombre_archivo, archivo_abierto->puntero);
            return true;
        }
    }
}

bool execute_ftruncate(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    t_instruccion *instruccion = instruccion_duplicate(pcontexto_desalojo->motivo_desalojo);
    char *archivo = list_get(instruccion->parametros, 0);
    // Armo la tarea para el hilo
    t_pcb_inst_status *arg = malloc(sizeof(t_pcb_inst_status));
    arg->pcb = pcb;
    arg->instruccion = instruccion;
    arg->status_expected = FILE_TRUNCATED;
    // Genero el hilo de trabajo
    pthread_create(&thr_file, 0, file_processing, (void *)arg);
    log_info(logger_main,
             "PID: %d - Truncar Archivo: %s - Tamanio: %d",
             pcb->pid, archivo, atoi(list_get(instruccion->parametros, 1)));

    pcb->next_queue = QBLOCK;
    log_info(logger_main, "PID: %d - Bloqueado por: %s", pcb->pid, archivo);
    return false;
}

void *file_processing(void *args)
{
    t_pcb_inst_status *pcb_inst_status = (t_pcb_inst_status *)args;
    bool finish = false;
    while (!finish && !end_program_flag)
    {
        if (!compactation_flag)
        {
            pthread_mutex_lock(&mutex_fs_connection);
            send_instruccion(modules_client->filesystem_client_socket, pcb_inst_status->instruccion, logger_aux);
            t_package *package = get_package(modules_client->filesystem_client_socket, logger_aux);
            pthread_mutex_unlock(&mutex_fs_connection);
            switch (package->operation_code)
            {
            case STATUS_CODE:
                t_status_code code = get_status_code(package);
                if (code == pcb_inst_status->status_expected)
                    move_pcb_from_to(pcb_inst_status->pcb, QBLOCK, QREADY);
                else
                {
                    pcb_inst_status->pcb->exit_status = code;
                    move_pcb_from_to(pcb_inst_status->pcb, QBLOCK, QEXIT);
                    EXIT(pcb_inst_status->pcb);
                }
                break;
            default:
                pcb_inst_status->pcb->exit_status = ERROR;
                move_pcb_from_to(pcb_inst_status->pcb, QBLOCK, QEXIT);
                EXIT(pcb_inst_status->pcb);
                break;
            }
            package_destroy(package);
            finish = true;
        }
        sleep(1);
    }
    if (pcb_inst_status->instruccion->identificador == I_F_READ || pcb_inst_status->instruccion->identificador == I_F_WRITE)
        list_remove(pcb_inst_status->instruccion->parametros, 3);
    instruccion_destroy(pcb_inst_status->instruccion);
    free(pcb_inst_status);
    pthread_exit(0);
}

// EXECUTE MEMORIA

bool execute_create_segment(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    bool status = false;
    t_instruccion *instruccion = pcontexto_desalojo->motivo_desalojo;
    if (pcontexto_desalojo->status_code == SEGMENTATION_FAULT)
    {
        log_info(logger_main, "PID: %d - Crear Segmento - Error de Segmentacion", pcb->pid);
        pcb->exit_status = SEGMENTATION_FAULT;
        pcb->next_queue = QEXIT;
        return false;
    }
    t_pid_instruccion *pid_instruccion = malloc(sizeof(t_pid_instruccion));
    pid_instruccion->instruccion = instruccion;
    pid_instruccion->pid = pcb->pid;
    send_pid_instruccion(modules_client->memory_client_socket, pid_instruccion, logger_aux);
    free(pid_instruccion);
    t_package *package = get_package(modules_client->memory_client_socket, logger_aux);
    switch (package->operation_code)
    {
    case STATUS_CODE:
        t_status_code code = get_status_code(package);
        if (code == OUT_OF_MEMORY)
        {
            pcb->exit_status = OUT_OF_MEMORY;
            pcb->next_queue = QEXIT;
        }
        else if (code == COMPACTATION_REQUIRED)
        {
            compactar();
            status = execute_create_segment(pcb, pcontexto_desalojo);
        }
        break;
    case SEGMENTO:
        t_segment *nuevo_segmento = get_segment(package);
        list_add(pcb->segments_table->segment_list, nuevo_segmento);
        log_info(logger_main, "PID: %d - Crear Segmento - Id: %d - Tamanio: %d", pcb->pid, nuevo_segmento->id, nuevo_segmento->size);
        status = true;
        break;
    default:
        log_error(logger_main, "PID: %d - Crear Segmento - Error al recibir respuesta de memoria", pcb->pid);
        pcb->exit_status = ERROR;
        pcb->next_queue = QEXIT;
        break;
    }
    package_destroy(package);
    return status;
}

bool execute_delete_segment(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo)
{
    bool status = false;
    t_instruccion *instruccion = pcontexto_desalojo->motivo_desalojo;
    t_pid_instruccion *pid_instruccion = malloc(sizeof(t_pid_instruccion));
    pid_instruccion->instruccion = instruccion;
    pid_instruccion->pid = pcb->pid;
    send_pid_instruccion(modules_client->memory_client_socket, pid_instruccion, logger_aux);
    free(pid_instruccion);
    t_package *package = get_package(modules_client->memory_client_socket, logger_aux);
    switch (package->operation_code)
    {
    case TSEGMENTOS:
        t_segments_table *tabla_actualizada = get_tsegmento(package);
        segments_table_destroy(pcb->segments_table);
        pcb->segments_table = tabla_actualizada;
        log_info(logger_main, "PID: %d - Eliminar Segmento - Id Segmento: %d", pcb->pid, atoi(list_get(instruccion->parametros, 0)));
        status = true;
        break;
    default:
        log_error(logger_main, "PID: %d - Eliminar Segmento - Error al recibir respuesta de memoria", pcb->pid);
        pcb->exit_status = ERROR;
        pcb->next_queue = QEXIT;
        break;
    }
    package_destroy(package);
    return status;
}

void compactar()
{
    compactation_flag = true;
    log_info(logger_main, "Compactacion: Esperando Fin de Operacion de FS");
    pthread_mutex_lock(&mutex_fs_connection); // Prohibo la comunicacion con FS
    fs_blocked = true;
    log_info(logger_main, "Compactacion: Se solicito compactacion");
    send_compactar(modules_client->memory_client_socket, logger_aux);
    t_package *package = get_package(modules_client->memory_client_socket, logger_aux);
    switch (package->operation_code)
    {
    case LTSEGMENTOS:
        t_list *tablas_actualizadas = get_ltsegmentos(package);
        actualizar_tablas(tablas_actualizadas);
        log_info(logger_main, "Compactacion: Se finalizo el proceso de compactacion");
        list_destroy(tablas_actualizadas);
        break;
    default:
        log_error(logger_main, "Compactacion: Error al recibir respuesta de memoria");
        end_program_flag = true;
        break;
    }
    package_destroy(package);
    pthread_mutex_unlock(&mutex_fs_connection); // Habilito la comunicacion con FS
    fs_blocked = false;
    compactation_flag = false;
}
