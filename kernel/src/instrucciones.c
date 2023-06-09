#include "instrucciones.h"

void actualizar_tablas(t_list *tablas_actualizadas)
{    
    for(int i = 0; i< list_size(tablas_actualizadas); i++){
        t_segments_table *aux_table = list_get(tablas_actualizadas, i);
        int j = find_pcb_index(all_pcb, aux_table->pid);
        ((t_pcb *)list_get(all_pcb, j))->segments_table = aux_table;
    }
}

bool execute_create_segment(t_instruccion *instruccion, t_pcb *pcb)
{   
    t_pid_instruccion * p = malloc(sizeof(t_pid_instruccion));
    p->instruccion = instruccion;
    p->pid = pcb->pid;

    // Envio a memoria instruccion y pid
    send_pid_instruccion(modules_client->memory_client_socket, p, logger_aux);

    uint32_t segment_id = atoi(list_get(instruccion->parametros,0));
    uint32_t segment_size = atoi(list_get(instruccion->parametros,1));
    
    t_package *paquete = get_package(modules_client->memory_client_socket, logger_aux);
    t_status_code code = get_status_code(paquete);

    switch(code){
        case SUCCESS: 
            t_package *p= get_package(modules_client->memory_client_socket, logger_aux);
            t_address base_nuevo_segmento = get_address(p); 
            t_segment* nuevo_segmento = malloc(sizeof(t_segment));
            nuevo_segmento->base_address = base_nuevo_segmento;
            nuevo_segmento->size = segment_id;
            nuevo_segmento->id = segment_size;
            list_add(pcb->segments_table->segment_list, nuevo_segmento);
            log_info(logger_main,"PID: %d - Crear Segmento - Id: %d - Tamanio: %d", pcb->pid, segment_id, segment_size);
            return true;
            break;

        case OUT_OF_MEMORY: 
            execute_exit(pcb, "OUTOFMEMORY");
            return false;
            break;

        case COMPACTATION_REQUIRED:
            compactar(pcb);
            log_info(logger_main, "Se finalizo el proceso de compactacion");
            t_package *p2 = get_package(modules_client->memory_client_socket, logger_aux);
            t_list *tablas_actualizadas = get_ltsegmentos(p2);
            actualizar_tablas(tablas_actualizadas);
            // vuelvo a ejecutar create_segment
            execute_create_segment(instruccion, pcb);
    }
    return false;
}

void compactar(t_pcb *pcb)
{
    log_info(logger_main, "Compactacion: Se solicito compactacion");
    int i = find_pcb_index(queues->BLOCK, pcb->pid);
    while(i != -1){ // esta en la cola de bloqueados
        log_info(logger_main, "Compactacion: Esperando Fin de Operacion de FS");
    }
    return;
}

bool execute_delete_segment(t_instruccion *instruccion, t_pcb *pcb)
{
    t_pid_instruccion *pid_instruccion = malloc(sizeof(t_pid_instruccion));
    pid_instruccion->instruccion = instruccion;
    pid_instruccion->pid = pcb->pid;

    // Envio a memoria instruccion y pid
    send_pid_instruccion(modules_client->memory_client_socket, pid_instruccion, logger_aux);

    t_package* p = get_package(modules_client->memory_client_socket, logger_aux);
    t_segments_table *tabla_actualizada = get_tsegmento(p);
    pcb->segments_table = tabla_actualizada;

    log_info(logger_main,"PID: %d - Eliminar Segmento - Id: %d", pcb->pid, atoi(list_get(instruccion->parametros,0)));

    return true;
}

void execute_fread(t_instruccion *instruccion, t_pcb *pcb)
{
    add_pcb_to_queue(QBLOCK, pcb);   
    send_instruccion(modules_client->filesystem_client_socket, instruccion, logger_aux);

    char *archivo = list_get(instruccion->parametros,0);
    int direccion = atoi(list_get(instruccion->parametros,1));
    int tamanio = atoi(list_get(instruccion->parametros,2));

    log_info(logger_main,"PID: %d - Leer Archivo: %s - Puntero:  - DIreccion Memoria: %d - Tamanio: %d", pcb->pid, archivo, direccion, tamanio); //FALTA PUNTERO DESPUES DE FSEEK, HAY Q HACERLO GLOBAL
 
    t_package *paquete = get_package(modules_client->memory_client_socket, logger_aux);
    t_status_code code = get_status_code(paquete);
    if(code == FILE_READED){
        int index_pendiente = find_pcb_index(queues->BLOCK, pcb->pid);
        pop_pcb_from_queue_by_index(QBLOCK, index_pendiente);
        add_pcb_to_queue(QREADY, pcb);

    }
    return;
}

void execute_fwrite(t_instruccion *instruccion, t_pcb *pcb)
{
    add_pcb_to_queue(QBLOCK, pcb);   
    send_instruccion(modules_client->filesystem_client_socket, instruccion, logger_aux);

    char *archivo = list_get(instruccion->parametros,0);
    int direccion = atoi(list_get(instruccion->parametros,1));
    int tamanio = atoi(list_get(instruccion->parametros,2));

    log_info(logger_main,"PID: %d - Leer Archivo: %s - Puntero:  - DIreccion Memoria: %d - Tamanio: %d", pcb->pid, archivo, direccion, tamanio);//FALTA PUNTERO DESPUES DE FSEEK, HAY Q HACERLO GLOBAL
  
    t_package *paquete = get_package(modules_client->memory_client_socket, logger_aux);
    t_status_code code = get_status_code(paquete);
    if(code == FILE_READED){
        int index_pendiente = find_pcb_index(queues->BLOCK, pcb->pid);
        pop_pcb_from_queue_by_index(QBLOCK, index_pendiente);
        add_pcb_to_queue(QREADY, pcb);

    }
    return;
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

void execute_exit(t_pcb *pcb, char *motivo)
{
    log_info(logger_main, "Finaliza el proceso %d - Motivo: %s", pcb->pid, motivo);
    add_pcb_to_queue(QEXIT, pcb);
}

void execute_to_ready(t_pcb *pcb)
{
    pcb->tiempo_llegada_ready = temporal_create();
    add_pcb_to_queue(QREADY, pcb);
}