#include "instrucciones.h"

void actualizar_tablas(t_list *tablas_actualizadas)
{
    for (int i = 0; i < list_size(tablas_actualizadas); i++)
    {
        t_segments_table *tabla_actualizada = list_get(tablas_actualizadas, i);
        t_pcb *pcb = list_get(all_pcb, find_pcb_index(all_pcb, tabla_actualizada->pid));
        segments_table_destroy(pcb->segments_table);
        pcb->segments_table = tabla_actualizada;
    }
}

bool execute_create_segment(t_instruccion *instruccion, t_pcb *pcb)
{
    bool status = false;
    t_pid_instruccion *p = malloc(sizeof(t_pid_instruccion));
    p->instruccion = instruccion;
    p->pid = pcb->pid;

    // Envio a memoria instruccion y pid
    send_pid_instruccion(modules_client->memory_client_socket, p, logger_aux);

    uint32_t segment_id = atoi(list_get(instruccion->parametros, 0));
    uint32_t segment_size = atoi(list_get(instruccion->parametros, 1));

    t_package *paquete = get_package(modules_client->memory_client_socket, logger_aux);

    switch (paquete->operation_code)
    {
    case STATUS_CODE:
        t_status_code code = get_status_code(paquete);
        if (code == OUT_OF_MEMORY)
        {
            execute_exit(pcb, OUT_OF_MEMORY);
        }
        else if (code == COMPACTATION_REQUIRED)
        {
            int i = find_pcb_index(queues->BLOCK, pcb->pid);
            if (i != -1)
            {
                log_info(logger_main, "Compactacion: Esperando Fin de Operacion de FS");
                while (i != -1)
                    i = find_pcb_index(queues->BLOCK, pcb->pid);
            }
            compactar();
            // vuelvo a ejecutar create_segment
            status = execute_create_segment(instruccion, pcb);
        }
        break;
    case SEGMENTO:
        t_segment *nuevo_segmento = get_segment(paquete);
        list_add(pcb->segments_table->segment_list, nuevo_segmento);
        log_info(logger_main, "PID: %d - Crear Segmento - Id: %d - Tamanio: %d", pcb->pid, segment_id, segment_size);
        status = true;
        break;
    default:
        log_error(logger_main, "Error al crear segmento");
        break;
    }
    package_destroy(paquete);
    free(p);
    return status;
}

void compactar()
{
    log_info(logger_main, "Compactacion: Se solicito compactacion");
    send_compactar(modules_client->memory_client_socket, logger_aux);
    t_package *paquete = get_package(modules_client->memory_client_socket, logger_aux);
    log_warning(logger_aux, "Compactacion: Se recibio paquete code op %d", paquete->operation_code);
    t_list *tablas_actualizadas = get_ltsegmentos(paquete);
    actualizar_tablas(tablas_actualizadas);
    log_info(logger_main, "Se finalizo el proceso de compactacion");
    list_destroy(tablas_actualizadas);
    package_destroy(paquete);
    return;
}

bool execute_delete_segment(t_instruccion *instruccion, t_pcb *pcb)
{
    t_pid_instruccion *pid_instruccion = malloc(sizeof(t_pid_instruccion));
    pid_instruccion->instruccion = instruccion;
    pid_instruccion->pid = pcb->pid;

    // Envio a memoria instruccion y pid
    send_pid_instruccion(modules_client->memory_client_socket, pid_instruccion, logger_aux);

    t_package *p = get_package(modules_client->memory_client_socket, logger_aux);
    t_segments_table *tabla_actualizada = get_tsegmento(p);
    segments_table_destroy(pcb->segments_table);
    pcb->segments_table = tabla_actualizada;

    log_info(logger_main, "PID: %d - Eliminar Segmento - Id: %d", pcb->pid, atoi(list_get(instruccion->parametros, 0)));

    free(pid_instruccion);
    package_destroy(p);

    return true;
}

bool execute_fopen(t_instruccion *instruccion, t_pcb *pcb)
{
    bool status = false;
    char *nombre_archivo = list_get(instruccion->parametros, 0);
    t_recurso *archivo = buscar_archivo(nombre_archivo);
    if (archivo == NULL) // Se consulta a FS si existe o no
    {
        send_instruccion(modules_client->filesystem_client_socket, instruccion, logger_aux);
        t_package *paquete = get_package(modules_client->filesystem_client_socket, logger_aux);
        t_status_code code = get_status_code(paquete);
        if (code == FILE_OPEN || code == FILE_CREATED)
        {
            log_info(logger_main, "PID: %d - Crear Archivo: %s", pcb->pid, nombre_archivo);
            // Creo el archivo
            t_recurso *nuevo_archivo = crear_archivo(nombre_archivo);
            list_add(archivos_abiertos, nuevo_archivo);
            // Asigno el archivo al pcb
            t_archivo_abierto *archivo_abierto = malloc(sizeof(t_archivo_abierto));
            archivo_abierto->archivo = nuevo_archivo;
            archivo_abierto->puntero = 0;
            list_add(pcb->open_files_table, archivo_abierto);
            nuevo_archivo->instancias--;
            status = true;
        }
        else
        {
            log_error(logger_main, "Error al crear archivo");
            execute_exit(pcb, code);
        }
        package_destroy(paquete);
    }
    else
    {
        // Si existe es porque otro lo posee asignado
        t_archivo_abierto *archivo_abierto = malloc(sizeof(t_archivo_abierto));
        archivo_abierto->archivo = archivo;
        archivo_abierto->puntero = 0;
        list_add(pcb->open_files_table, archivo_abierto);
        add_pcb_to_queue(QBLOCK, pcb);
        list_add(archivo->lista_bloqueados, pcb);
        archivo->instancias--;
    }
    return status;
}

t_archivo_abierto *buscar_archivo_abierto(t_pcb *pcb, char *nombre_archivo)
{
    for (int i = 0; i < list_size(pcb->open_files_table); i++)
    {
        t_archivo_abierto *archivo_abierto = list_get(pcb->open_files_table, i);
        if (string_equals_ignore_case(archivo_abierto->archivo->recurso, nombre_archivo))
            return archivo_abierto;
    }
    return NULL;
}

int find_archivo_abierto_index(t_pcb *pcb, char *nombre_archivo)
{
    for (int i = 0; i < list_size(pcb->open_files_table); i++)
    {
        t_archivo_abierto *archivo_abierto = list_get(pcb->open_files_table, i);
        if (string_equals_ignore_case(archivo_abierto->archivo->recurso, nombre_archivo))
            return i;
    }
    return -1;
}

int find_archivo_index(char *nombre_archivo)
{
    for (int i = 0; i < list_size(archivos_abiertos); i++)
    {
        t_recurso *archivo = list_get(archivos_abiertos, i);
        if (string_equals_ignore_case(archivo->recurso, nombre_archivo))
            return i;
    }
    return -1;
}

bool execute_fclose(t_instruccion *instruccion, t_pcb *pcb)
{
    char *nombre_archivo = list_get(instruccion->parametros, 0);
    t_recurso *archivo = buscar_archivo(nombre_archivo);
    if (archivo == NULL)
    {
        log_error(logger_main, "El archivo %s no existe", nombre_archivo);
        return false;
    }
    else
    {
        t_archivo_abierto *archivo_abierto = buscar_archivo_abierto(pcb, nombre_archivo);
        if (archivo_abierto == NULL)
        {
            log_error(logger_main, "El archivo %s no esta abierto", nombre_archivo);
            return false;
        }
        else
        {
            int index_archivo_abierto = find_archivo_abierto_index(pcb, nombre_archivo);
            t_archivo_abierto *archivo_abierto = list_remove(pcb->open_files_table, index_archivo_abierto);
            free(archivo_abierto);
            archivo->instancias++;
            if (archivo->instancias > 0)
            {
                int index_archivo = find_archivo_index(nombre_archivo);
                list_remove(archivos_abiertos, index_archivo);
                free_recurso(archivo);
            }
            else
            {
                t_pcb *pendiente = remove_pcb_from_queue_resourse(archivo);
                int index_pendiente = find_pcb_index(queues->BLOCK, pendiente->pid);
                pop_pcb_from_queue_by_index(QBLOCK, index_pendiente);
                add_pcb_to_queue(QREADY, pendiente);
            }
            return true;
        }
    }
    return false;
}

bool execute_fseek(t_instruccion *instruccion, t_pcb *pcb)
{
    char *nombre_archivo = list_get(instruccion->parametros, 0);
    t_recurso *archivo = buscar_archivo(nombre_archivo);
    if (archivo == NULL)
    {
        log_error(logger_main, "El archivo %s no existe", nombre_archivo);
        return false;
    }
    else
    {
        t_archivo_abierto *archivo_abierto = buscar_archivo_abierto(pcb, nombre_archivo);
        if (archivo_abierto == NULL)
        {
            log_error(logger_main, "El archivo %s no esta abierto", nombre_archivo);
            return false;
        }
        else
        {
            archivo_abierto->puntero = atoi(list_get(instruccion->parametros, 1));
            log_info(logger_main, "PID: %d - Actualizar puntero Archivo: %s - Puntero %d", pcb->pid, nombre_archivo, archivo_abierto->puntero);
            return true;
        }
    }
    return false;
}

void execute_fread(t_instruccion *instruccion, t_pcb *pcb)
{
    add_pcb_to_queue(QBLOCK, pcb);

    char *archivo = list_get(instruccion->parametros, 0);
    int direccion = atoi(list_get(instruccion->parametros, 1));
    int tamanio = atoi(list_get(instruccion->parametros, 2));
    t_archivo_abierto *archivo_abierto = buscar_archivo_abierto(pcb, archivo);

    list_add(instruccion->parametros, string_itoa(archivo_abierto->puntero));

    send_instruccion(modules_client->filesystem_client_socket, instruccion, logger_aux);

    log_info(logger_main, "PID: %d - Leer Archivo: %s - Puntero: %d - DIreccion Memoria: %d - Tamanio: %d", pcb->pid, archivo, archivo_abierto->puntero, direccion, tamanio);

    t_pcb_file_status *arg = malloc(sizeof(t_pcb_file_status));
    arg->pid = pcb->pid;
    arg->status_expected = FILE_READ;
    pthread_create(&thr_file, 0, file_processing, (void *)arg);
}

void *file_processing(void *args)
{
    t_pcb_file_status *pcb_file_status = (t_pcb_file_status *)args;
    t_package *paquete = get_package(modules_client->filesystem_client_socket, logger_aux);
    int index = find_pcb_index(queues->BLOCK, pcb_file_status->pid);
    t_pcb *pcb = pop_pcb_from_queue_by_index(QBLOCK, index);
    t_status_code code = get_status_code(paquete);
    if (code == pcb_file_status->status_expected)
    {
        add_pcb_to_queue(QREADY, pcb);
    }
    else
    {
        execute_exit(pcb, code);
    }
    package_destroy(paquete);
    free(pcb_file_status);
    pthread_exit(0);
}

void execute_fwrite(t_instruccion *instruccion, t_pcb *pcb)
{
    add_pcb_to_queue(QBLOCK, pcb);

    char *archivo = list_get(instruccion->parametros, 0);
    int direccion = atoi(list_get(instruccion->parametros, 1));
    int tamanio = atoi(list_get(instruccion->parametros, 2));
    t_archivo_abierto *archivo_abierto = buscar_archivo_abierto(pcb, archivo);

    list_add(instruccion->parametros, string_itoa(archivo_abierto->puntero));

    send_instruccion(modules_client->filesystem_client_socket, instruccion, logger_aux);

    log_info(logger_main, "PID: %d - Escribir Archivo: %s - Puntero: %d - DIreccion Memoria: %d - Tamanio: %d", pcb->pid, archivo, archivo_abierto->puntero, direccion, tamanio);

    t_pcb_file_status *arg = malloc(sizeof(t_pcb_file_status));
    arg->pid = pcb->pid;
    arg->status_expected = FILE_WRITTEN;
    pthread_create(&thr_file, 0, file_processing, (void *)arg);
}

void execute_ftruncate(t_instruccion *instruccion, t_pcb *pcb)
{
    add_pcb_to_queue(QBLOCK, pcb);

    char *archivo = list_get(instruccion->parametros, 0);
    int tamanio = atoi(list_get(instruccion->parametros, 1));

    send_instruccion(modules_client->filesystem_client_socket, instruccion, logger_aux);

    log_info(logger_main, "PID: %d - Archivo: %s - Tamanio: %d", pcb->pid, archivo, tamanio);

    t_pcb_file_status *arg = malloc(sizeof(t_pcb_file_status));
    arg->pid = pcb->pid;
    arg->status_expected = FILE_TRUNCATED;
    pthread_create(&thr_file, 0, file_processing, (void *)arg);
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
        add_pcb_to_queue(QEXIT, pcb);
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
        add_pcb_to_queue(QBLOCK, pcb);
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
        add_pcb_to_queue(QEXIT, pcb);
        return false;
    }

    recurso->instancias++;
    log_info(logger_main, "PID: %d - Signal: %s - Instancias: %d", pcb->pid, recurso->recurso, recurso->instancias);
    if (recurso->instancias <= 0)
    {
        t_pcb *pendiente = remove_pcb_from_queue_resourse(recurso);
        int index_pendiente = find_pcb_index(queues->BLOCK, pendiente->pid);
        pop_pcb_from_queue_by_index(QBLOCK, index_pendiente);
        add_pcb_to_queue(QREADY, pcb);
    }
    // MANDAR A EJECUTAR DEVUELTA CPU
    return true;
}

void execute_io(int tiempo, t_pcb *pcb)
{
    add_pcb_to_queue(QBLOCK, pcb);
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
    t_pcb *pcb = pop_pcb_from_queue_by_index(QBLOCK, index);
    add_pcb_to_queue(QREADY, pcb);
    free(io_pcb);
    pthread_exit(0);
}

char *get_status_code_string(t_status_code sc)
{
    switch (sc)
    {
    case SUCCESS:
        return "SUCCESS";
    case OUT_OF_MEMORY:
        return "OUT OF MEMORY";
    case SEGMENTATION_FAULT:
        return "SEGMENTATION FAULT";
    default:
        return "UNKNOWN";
    }
}

void execute_exit(t_pcb *pcb, t_status_code sc)
{
    char *status_code = get_status_code_string(sc);
    log_info(logger_main, "Finaliza el proceso %d - Motivo: %s", pcb->pid, status_code);
    t_pid_status *pid_status = malloc(sizeof(t_pid_status));
    pid_status->pid = pcb->pid;
    pid_status->status = PROCESS_END;
    send_pid_status(modules_client->memory_client_socket, pid_status, logger_aux);
    pcb->exit_status = sc;
    add_pcb_to_queue(QEXIT, pcb);
    free(pid_status);
}

void execute_yield(t_pcb *pcb)
{
    temporal_destroy(pcb->tiempo_llegada_ready);
    pcb->tiempo_llegada_ready = temporal_create();
    add_pcb_to_queue(QREADY, pcb);
}
