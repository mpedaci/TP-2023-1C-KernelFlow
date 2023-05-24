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


typedef struct{
    char* recurso;
    int instancias;
    t_list* lista_bloqueados;
}t_recurso;

void cargar_recursos(t_recurso** recursos){
    for(int i = 0; i < list_size(config_kernel->recursos); i++){
        recursos[i]->recurso = (char*)list_get(config_kernel->recursos, i);
        recursos[i]->instancias = list_get(config_kernel->instancias_recursos, i);
        recursos[i]->lista_bloqueados = list_create();
    }
}

void execute(){
    t_pcb *pcb = queue_pop(queues->EXEC);
    t_pcontexto *pcontexto = create_pcontexto_from_pcb(pcb);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux);
    t_package *p = get_package(modules_client->cpu_client_socket, logger_aux);
    t_pcontexto_desalojo *pcontexto_response = get_pcontexto_desalojo(p);
    update_pcb_from_pcontexto(pcb, pcontexto_response);

    t_instruccion* instruccion_desalojo = pcontexto_response->motivo_desalojo;
    char* recurso_solicitado = instruccion_desalojo->parametros[1];

    t_recurso** recursos;
    cargar_recursos(recursos);

        switch (instruccion_desalojo->identificador){

            case I_WAIT:
                execute_wait(recurso_solicitado, recursos, pcb);
                break;
            case I_SIGNAL:
                execute_signal(recurso_solicitado, recursos, pcb);
                break;
            /*case I_CREATE_SEGMENT:
                execute_create_segment(pcb->segments_table->segment_size, pcb);
                break;
            case I_DELETE_SEGMENT:
                execute_delete_segment(pcb->segments_table->id, pcb);*/
            default:
            break;
        }
    
      free(pcontexto);
      free(pcontexto_response);
}

/*void execute_create_segment(uint32_t segment_size, t_pcb* pcb){
    // Enviar a memoria tamanio para crear segmento
    send_uint32_t(modules_client->memory_client_socket, segment_size, logger_main);


    // Devolver contexto a cpu
    t_pcontexto *pcontexto = create_pcontexto_from_pcb(pcb);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux);
}

void execute_delete_segment(uint32_t id_segment, t_pcb* pcb){
    // Enviar a memoria id del segmento a eliminar
    send_uint32_t(modules_client->memory_client_socket, id_segment, logger_main);

    t_package* p = get_package(modules_client->memory_client_socket, logger_aux);
    t_segments_table *tabla_actualizada = get_tsegmento(p);

    //Devolver contexto a cpu
    t_pcontexto *pcontexto = create_pcontexto_from_pcb(pcb);
    send_pcontexto(modules_client->cpu_client_socket, pcontexto, logger_aux);
}*/

/*bool send_uint32_t(int socket, uint32_t uint32, t_log* logger){
    t_buffer* buffer = uint32_t_create_buffer(uint32);
    t_package* paquete = package_create(buffer, SEGMENT_SIZE);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);

    return res;
}

// Usar en memoria
uint32_t* get_uint32_t(t_package* paquete){ 
    uint32_t* uint32 = uint32_t_create_from_buffer(paquete->buffer);
    return uint32;
}

t_buffer* uint32_t_create_buffer(uint32_t uint32){

    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t);
    void* stream = malloc(buffer->size);
  
    memcpy(stream, &uint32, sizeof(uint32_t));
    buffer->stream = stream;

    return buffer;
}

uint32_t* uint32_t_create_from_buffer(t_buffer* buffer){

    uint32_t uint32;
    void* stream = buffer->stream;

    memcpy(&(uint32), stream, sizeof(uint32_t));

    return uint32;
}
*/


void execute_wait(char* recurso_solicitado, t_recurso** recursos, t_pcb *pcb){

    int posicion_aux = -1;
    int cantidad_recursos = list_size(config_kernel->recursos);

    // creo un semaforo por cada recurso y un mutex para la cola de bloqueados
    sem_t sem_recursos[cantidad_recursos];
    sem_t sem_mutex_colas_bloqueados[cantidad_recursos];

    for(int i = 0; i < cantidad_recursos; i++){
        sem_init(&sem_recursos[i], 0, recursos[i]->instancias);
    }

    for(int i = 0; i < cantidad_recursos; i++){
        sem_init(&sem_mutex_colas_bloqueados[i], 0, 1);
    }

    // busco la posicion del recurso
    for(int i=0; i< cantidad_recursos; i++){
        if(string_equals_ignore_case(recurso_solicitado, recursos[i]->recurso)){
            posicion_aux = i;
        }
    }

    if(posicion_aux = -1){
        log_info(logger_main, "El recurso %s no existe", recurso_solicitado);
        queue_push(queues->EXIT, pcb);
    }

    sem_wait(&sem_recursos[posicion_aux]);

    recursos[posicion_aux]->instancias--;

    if(recursos[posicion_aux]->instancias < 0){
        sem_post(&sem_recursos[posicion_aux]);
        sem_wait(&sem_mutex_colas_bloqueados[posicion_aux]);
        
        queue_add(recursos[posicion_aux]->lista_bloqueados, pcb);
        
        log_info(logger_main,“PID: %d - Bloqueado por: %s”, pcb->pid, recursos[posicion_aux]->recurso);
 
        sem_post(&sem_mutex_colas_bloqueados[posicion_aux]);
    } else
        sem_post(&sem_recursos[posicion_aux]);
    
    log_info(logger_main,"PID: %d - Wait: %s - Instancias: %d", pcb->pid, recursos[posicion_aux]->recurso, recursos[posicion_aux]->instancias);

        // destruir semaforos
    for (int i = 0; i < cantidad_recursos; i++) {
        sem_destroy(&sem_recursos[i]);
    }
    for (int i = 0; i < cantidad_recursos; i++) {
        sem_destroy(&sem_mutex_colas_bloqueados[i]);
    }
}

void execute_signal(char* recurso_solicitado, t_recurso** recursos, t_pcb *pcb){

    int posicion_aux = -1;
    int cantidad_recursos = list_size(config_kernel->recursos);

    sem_t sem_recursos[cantidad_recursos];
    sem_t sem_mutex_colas_bloqueados[cantidad_recursos];

    for(int i = 0; i < cantidad_recursos; i++){
        sem_init(&sem_recursos[i], 0, recursos[i]->instancias);
    }

    for(int i = 0; i < cantidad_recursos; i++){
        sem_init(&sem_mutex_colas_bloqueados[i], 0, 1);
    }

    // busco la posicion del recurso
    for(int i=0; i< cantidad_recursos; i++){
        if(string_equals_ignore_case(recurso_solicitado, recursos[i]->recurso)){
            posicion_aux = i;
        }
    }

    if(posicion_aux = -1){
        log_info(logger_main, "El recurso %s no existe", recurso_solicitado);
        queue_push(queues->EXIT, pcb);
    }

    sem_wait(&sem_recursos[posicion_aux]);

    recursos[posicion_aux]->instancias++;

    sem_wait(&sem_mutex_colas_bloqueados[posicion_aux]);
 
    t_pcb* proceso = queue_pop(recursos[posicion_aux]->lista_bloqueados);
    queue_push(queues->EXEC, proceso);

    sem_post(&sem_mutex_colas_bloqueados[posicion_aux]);
    sem_post(&sem_recursos[posicion_aux]);
    
    log_info(logger_main,"PID: %d - Signal: %s - Instancias: %d", pcb->pid, recursos[posicion_aux]->recurso, recursos[posicion_aux]->instancias);

    for (int i = 0; i < cantidad_recursos; i++) {
        sem_destroy(&sem_recursos[i]);
    }
    for (int i = 0; i < cantidad_recursos; i++) {
        sem_destroy(&sem_mutex_colas_bloqueados[i]);
    }
}



void inicializar_semaforos(){}
void finalizar_semaforos(){}
