#include "utils.h"

// AUXILIAR FUNCTIONS

void add_to_list(char **source, t_list *destiny)
{
    int i = 0;
    while (source[i] != NULL)
    {
        list_add(destiny, string_duplicate(source[i]));
        i++;
    }
}

void free_arr(char **arr)
{
    int i = 0;
    while (arr[i] != NULL)
    {
        free(arr[i]);
        i++;
    }
    free(arr);
}

// KERNEL FUNCTIONS

t_config_kernel *read_config(char *config_path, t_log *logger)
{
    t_config_kernel *kernel_config = malloc(sizeof(t_config_kernel));
    t_config *config = config_create(config_path);
    if (config == NULL)
    {
        log_error(logger, "No se pudo leer el archivo de configuración");
        return NULL;
    }
    // CLIENTE - SERVIDOR
    kernel_config->ip_memoria = string_duplicate(config_get_string_value(config, "IP_MEMORIA"));
    kernel_config->puerto_memoria = string_duplicate(config_get_string_value(config, "PUERTO_MEMORIA"));
    kernel_config->ip_filesystem = string_duplicate(config_get_string_value(config, "IP_FILESYSTEM"));
    kernel_config->puerto_filesystem = string_duplicate(config_get_string_value(config, "PUERTO_FILESYSTEM"));
    kernel_config->ip_cpu = string_duplicate(config_get_string_value(config, "IP_CPU"));
    kernel_config->puerto_cpu = string_duplicate(config_get_string_value(config, "PUERTO_CPU"));
    kernel_config->puerto_escucha = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA"));
    // PLANIFICADOR
    kernel_config->grado_max_multiprog = config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");
    kernel_config->algoritmo_planificacion = string_duplicate(config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
    kernel_config->estimacion_inicial = config_get_double_value(config, "ESTIMACION_INICIAL");
    kernel_config->hrrn_alfa = config_get_double_value(config, "HRRN_ALFA");
    // IO
    kernel_config->recursos = list_create();
    kernel_config->instancias_recursos = list_create();
    char **recursos = config_get_array_value(config, "RECURSOS");
    char **instancias_recursos = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    add_to_list(recursos, kernel_config->recursos);
    add_to_list(instancias_recursos, kernel_config->instancias_recursos);
    log_debug(logger, "Archivo de configuración leído correctamente");
    config_destroy(config);
    free_arr(recursos);
    free_arr(instancias_recursos);
    return kernel_config;
}

t_queues *create_queues()
{
    t_queues *queues = malloc(sizeof(t_queues));
    queues->NEW = list_create();
    queues->READY = list_create();
    queues->EXEC = list_create();
    queues->BLOCK = list_create();
    queues->EXIT = list_create();
    return queues;
}

t_registers *init_registers()
{
    t_registers *registers = malloc(sizeof(t_registers));

    registers->AX = malloc(4);
    registers->BX = malloc(4);
    registers->CX = malloc(4);
    registers->DX = malloc(4);
    registers->EAX = malloc(8);
    registers->EBX = malloc(8);
    registers->ECX = malloc(8);
    registers->EDX = malloc(8);
    registers->RAX = malloc(16);
    registers->RBX = malloc(16);
    registers->RCX = malloc(16);
    registers->RDX = malloc(16);

    char *zero = "0000000000000000";

    memcpy(registers->AX, zero, 4);
    memcpy(registers->BX, zero, 4);
    memcpy(registers->CX, zero, 4);
    memcpy(registers->DX, zero, 4);
    memcpy(registers->EAX, zero, 8);
    memcpy(registers->EBX, zero, 8);
    memcpy(registers->ECX, zero, 8);
    memcpy(registers->EDX, zero, 8);
    memcpy(registers->RAX, zero, 16);
    memcpy(registers->RBX, zero, 16);
    memcpy(registers->RCX, zero, 16);
    memcpy(registers->RDX, zero, 16);

    return registers;
}

t_pcb *pcb_create(uint32_t pid, t_list *instrucciones)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->pid = pid;
    pcb->instrucciones = instrucciones;
    pcb->program_counter = 0;
    pcb->registers = init_registers();
    pcb->segments_table = NULL;
    pcb->est_sig_rafaga = 0;
    t_temporal *temporal = temporal_create();
    pcb->tiempo_llegada_ready = temporal;
    pcb->tiempo_entrada_cpu = temporal;
    pcb->tiempo_salida_cpu = temporal;
    pcb->open_files_table = NULL;
    pcb->exit_status = SUCCESS;
    return pcb;
}

// END PROGRAM

void end_program(
    t_log *logger_main, t_log *logger_aux,
    t_config_kernel *config,
    t_modules_client *modules_client,
    t_queues *queues)
{
    // Modules Client destroy
    if (modules_client != NULL)
        free_modules_client(modules_client, logger_aux);

    log_debug(logger_aux, "Finalizando programa");
    // Logs destroy
    log_destroy(logger_main);
    log_destroy(logger_aux);
    // Kernel Config destroy
    if (config != NULL)
        free_config_kernel(config);
    // Queues destroy
    if (queues != NULL)
        free_queues(queues);
}

// FREE FUNCTIONS

void free_config_kernel(t_config_kernel *config)
{
    free(config->ip_memoria);
    free(config->puerto_memoria);
    free(config->ip_filesystem);
    free(config->puerto_filesystem);
    free(config->ip_cpu);
    free(config->puerto_cpu);
    free(config->puerto_escucha);
    free(config->algoritmo_planificacion);
    list_destroy_and_destroy_elements(config->recursos, free);
    list_destroy_and_destroy_elements(config->instancias_recursos, free);
    free(config);
}

void free_modules_client(t_modules_client *modules_client, t_log *logger)
{
    send_end(modules_client->cpu_client_socket, logger);
    send_end(modules_client->filesystem_client_socket, logger);
    send_end(modules_client->memory_client_socket, logger);
    socket_destroy(modules_client->cpu_client_socket);
    socket_destroy(modules_client->filesystem_client_socket);
    socket_destroy(modules_client->memory_client_socket);
    free(modules_client);
}

void free_registers(t_registers *registers)
{
    free(registers->AX);
    free(registers->BX);
    free(registers->CX);
    free(registers->DX);
    free(registers->EAX);
    free(registers->EBX);
    free(registers->ECX);
    free(registers->EDX);
    free(registers->RAX);
    free(registers->RBX);
    free(registers->RCX);
    free(registers->RDX);

    free(registers);
}

void free_segments_table(t_segments_table *s_table)
{
    list_destroy_and_destroy_elements(s_table->segment_list, free);
    free(s_table);
}

void free_pcb(t_pcb *pcb)
{
    free_lista_instrucciones(pcb->instrucciones);
    free_registers(pcb->registers);
    free_segments_table(pcb->segments_table);
    temporal_destroy(pcb->tiempo_llegada_ready);
    temporal_destroy(pcb->tiempo_entrada_cpu);
    temporal_destroy(pcb->tiempo_salida_cpu);
    // free(pcb->open_files_table);
    free(pcb);
}

void free_queues(t_queues *queues)
{
    list_destroy_and_destroy_elements(queues->NEW, (void *)free_pcb);
    list_destroy_and_destroy_elements(queues->READY, (void *)free_pcb);
    list_destroy_and_destroy_elements(queues->EXEC, (void *)free_pcb);
    list_destroy_and_destroy_elements(queues->BLOCK, (void *)free_pcb);
    list_destroy_and_destroy_elements(queues->EXIT, (void *)free_pcb);
    free(queues);
}

void free_instruccion(t_instruccion *instruccion)
{
    list_destroy_and_destroy_elements(instruccion->parametros, free);
    free(instruccion);
}

void free_lista_instrucciones(t_list *lista_instrucciones)
{
    list_destroy_and_destroy_elements(lista_instrucciones, (void *)free_instruccion);
}

void free_pcontexto_desalojo(t_pcontexto_desalojo *pcontexto)
{
    free_lista_instrucciones(pcontexto->instructions);
    free_instruccion(pcontexto->motivo_desalojo);
    free_registers(pcontexto->registers);
    free(pcontexto);
}

void free_pcontexto(t_pcontexto *pcontexto)
{
    free_lista_instrucciones(pcontexto->instructions);
    free_registers(pcontexto->registers);
    list_destroy_and_destroy_elements(pcontexto->segments, free);
    free(pcontexto);
}


void copy_registers(t_registers *dest, t_registers *src)
{
    memcpy(dest->AX, src->AX, 4);
    memcpy(dest->BX, src->BX, 4);
    memcpy(dest->CX, src->CX, 4);
    memcpy(dest->DX, src->DX, 4);

    memcpy(dest->EAX, src->EAX, 8);
    memcpy(dest->EBX, src->EBX, 8);
    memcpy(dest->ECX, src->ECX, 8);
    memcpy(dest->EDX, src->EDX, 8);

    memcpy(dest->RAX, src->RAX, 16);
    memcpy(dest->RBX, src->RBX, 16);
    memcpy(dest->RCX, src->RCX, 16);
    memcpy(dest->RDX, src->RDX, 16);
}

t_list *copy_instructions_list(t_list *instructions)
{
    t_list *new_list = list_create();
    for (int i = 0; i < list_size(instructions); i++)
    {
        t_instruccion *instruction = new_instruction(((t_instruccion *)list_get(instructions, i)));
        list_add(new_list, instruction);
    }
    return new_list;
}

t_instruccion *new_instruction(t_instruccion *instruccion)
{
    t_instruccion *new_instruction = malloc(sizeof(t_instruccion));
    new_instruction->identificador = instruccion->identificador;
    new_instruction->cant_parametros = instruccion->cant_parametros;
    new_instruction->parametros = list_create();
    for (int i = 0; i < instruccion->cant_parametros; i++)
    {
        char *param = string_duplicate(list_get(instruccion->parametros, i));
        list_add(new_instruction->parametros, param);
    }
    for (int i = 0; i < 4; i++)
        new_instruction->p_length[i] = instruccion->p_length[i];
    return new_instruction;
}

t_list *copy_segment_list(t_segments_table *segments_table)
{
    t_list *new_list = list_create();
    for (int i = 0; i < list_size(segments_table->segment_list); i++)
    {
        t_segment *segment = new_segment(((t_segment *)list_get(segments_table->segment_list, i)));
        list_add(new_list, segment);
    }
    return new_list;
}

t_segment *new_segment(t_segment *segment)
{
    t_segment *new_segment = malloc(sizeof(t_segment));
    new_segment->id = segment->id;
    new_segment->base_address = segment->base_address;
    new_segment->size = segment->size;
    return new_segment;
}