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

    // registers->AX = "000";
    // registers->BX = "000";
    // registers->CX = "000";
    // registers->DX = "000";
    // registers->EAX = "0000000";
    // registers->EBX = "0000000";
    // registers->ECX = "0000000";
    // registers->EDX = "0000000";
    // registers->RAX = "000000000000000";
    // registers->RBX = "000000000000000";
    // registers->RCX = "000000000000000";
    // registers->RDX = "000000000000000";

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
    pcb->tiempo_llegada_ready = temporal_create();
    pcb->open_files_table = NULL;
    return pcb;
}

// END PROGRAM

void end_program(
    t_log *logger_main, t_log *logger_aux,
    t_config_kernel *config,
    t_modules_client *modules_client,
    t_queues *queues)
{
    log_debug(logger_aux, "Finalizando programa");
    // Logs destroy
    log_destroy(logger_main);
    log_destroy(logger_aux);
    // Modules Client destroy
    if (modules_client != NULL)
        free_modules_client(modules_client);
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

void free_modules_client(t_modules_client *modules_client)
{
    socket_destroy(modules_client->cpu_client_socket);
    socket_destroy(modules_client->filesystem_client_socket);
    socket_destroy(modules_client->memory_client_socket);
    free(modules_client);
}

void free_queues(t_queues *queues)
{
    list_destroy(queues->NEW);
    list_destroy(queues->READY);
    list_destroy(queues->EXEC);
    list_destroy(queues->BLOCK);
    list_destroy(queues->EXIT);
    free(queues);
}