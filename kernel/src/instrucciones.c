#include "instrucciones.h"

void execute_create_segment(uint32_t segment_size, uint32_t segment_id, t_pcb* pcb){
    // Enviar a memoria tamanio para crear segmento y id
    send(modules_client->memory_client_socket, &segment_size, sizeof(uint32_t), NULL);
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
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux);
}

void compactar(){
    log_info(logger_main,"Compactacion: Se solicito compactacion");
    log_info(logger_main,"Compactacion: Esperando Fin de Operacion de FS");
    log_info(logger_main,"Se finalizo el proceso de compactacion");
}

void execute_delete_segment(uint32_t segment_id, t_pcb* pcb){
    // Enviar a memoria id del segmento a eliminar
    send(modules_client->memory_client_socket, &segment_id, sizeof(uint32_t), NULL);

    t_package* p = get_package(modules_client->memory_client_socket, logger_aux);
    t_segments_table *tabla_actualizada = get_tsegmento(p);
    pcb->segments_table = tabla_actualizada;

    log_info(logger_main,"PID: %d - Eliminar Segmento - Id: %d", pcb->pid, segment_id);

    // Devolver contexto a cpu
    t_pcontexto *pcontexto = create_pcontexto_from_pcb(pcb);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux);
}

void execute_wait(char* recurso_solicitado, t_recurso** recursos, t_pcb *pcb){

    int posicion_aux = -1;
    int cantidad_recursos = list_size(config_kernel->recursos);

    // Un semaforo por cada recurso y un mutex para la cola de bloqueados del mismo
    sem_t sem_recursos[cantidad_recursos];
    pthread_mutex_t mutex_colas_bloqueados[cantidad_recursos];

    for(int i = 0; i < cantidad_recursos; i++){
        sem_init(&sem_recursos[i], 0, recursos[i]->instancias);
    }

    for(int i = 0; i < cantidad_recursos; i++){
        pthread_mutex_init(&mutex_colas_bloqueados[i], NULL);
    }

    // busco la posicion del recurso
    for(int i=0; i< cantidad_recursos; i++){
        if(string_equals_ignore_case(recurso_solicitado, recursos[i]->recurso)){
            posicion_aux = i;
        }
    }

    if(posicion_aux = -1){
        log_info(logger_main, "El recurso %s no existe", recurso_solicitado);
        agregar_pcb_a_cola(pcb, mutex_exit, queues->EXIT);
    }

    sem_wait(&sem_recursos[posicion_aux]);

    recursos[posicion_aux]->instancias--;

    if(recursos[posicion_aux]->instancias < 0){
        sem_post(&sem_recursos[posicion_aux]);

        agregar_pcb_a_cola(pcb, mutex_colas_bloqueados[posicion_aux], recursos[posicion_aux]->lista_bloqueados);

        log_info(logger_main,"PID: %d - Bloqueado por: %s", pcb->pid, recursos[posicion_aux]->recurso);

    } else
        sem_post(&sem_recursos[posicion_aux]);
    
        log_info(logger_main,"PID: %d - Wait: %s - Instancias: %d", pcb->pid, recursos[posicion_aux]->recurso, recursos[posicion_aux]->instancias);

        // destruir semaforos
    for (int i = 0; i < cantidad_recursos; i++) {
        sem_destroy(&sem_recursos[i]);
    }
}

void execute_signal(char* recurso_solicitado, t_recurso** recursos, t_pcb *pcb){

    int posicion_aux = -1;
    int cantidad_recursos = list_size(config_kernel->recursos);

    // Un semaforo por recurso y un mutex para la cola de bloqueados del mismo
    sem_t sem_recursos[cantidad_recursos];
    pthread_mutex_t mutex_colas_bloqueados[cantidad_recursos];

    for(int i = 0; i < cantidad_recursos; i++){
        sem_init(&sem_recursos[i], 0, recursos[i]->instancias);
    }

    for(int i = 0; i < cantidad_recursos; i++){
        pthread_mutex_init(&mutex_colas_bloqueados[i], NULL);
    }

    // busco la posicion del recurso
    for(int i=0; i< cantidad_recursos; i++){
        if(string_equals_ignore_case(recurso_solicitado, recursos[i]->recurso)){
            posicion_aux = i;
        }
    }

    if(posicion_aux = -1){
        log_info(logger_main, "El recurso %s no existe", recurso_solicitado);
        agregar_pcb_a_cola(pcb, mutex_exit, queues->EXIT);
    }

    sem_wait(&sem_recursos[posicion_aux]);

    recursos[posicion_aux]->instancias++;
 
    t_pcb* proceso = quitar_pcb_de_cola(mutex_colas_bloqueados[posicion_aux], recursos[posicion_aux]->lista_bloqueados);
    
    agregar_pcb_a_cola(proceso, mutex_running, queues->EXEC);

    sem_post(&sem_recursos[posicion_aux]);
    
    log_info(logger_main,"PID: %d - Signal: %s - Instancias: %d", pcb->pid, recursos[posicion_aux]->recurso, recursos[posicion_aux]->instancias);

    for (int i = 0; i < cantidad_recursos; i++) {
        sem_destroy(&sem_recursos[i]);
    }
}


void execute_io(int tiempo, t_pcb* pcb){
    agregar_pcb_a_cola(pcb, mutex_blocked, queues->BLOCK);
    log_info(logger_main, "PID: %d - Ejecuta IO: %d", pcb->pid, tiempo);

    pthread_t hilo_IO;

    pthread_create(&hilo_IO, NULL, (void*) io, (void*)(int)tiempo);
    pthread_detach(hilo_IO);
}

void io(int tiempo){
    t_pcb* pcb = quitar_pcb_de_cola(mutex_blocked, queues->BLOCK);
    sleep(tiempo);
    agregar_pcb_a_cola(pcb, mutex_ready, queues->READY);
}

void execute_exit(t_pcb* pcb){
    agregar_pcb_a_cola(pcb, mutex_exit, queues->EXIT);
}

void inicializar_semaforos(){}
void finalizar_semaforos(){}
