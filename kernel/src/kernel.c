#include "kernel.h"

int main()
{
    // Para cierre seguro
    signal(SIGINT, sigintHandler);
    // 0. Inicializar loggers
    logger_main = log_create(logger_main_path, "KERNEL", true, LOG_LEVEL_INFO);
    logger_aux = log_create(logger_aux_path, "KERNEL AUX", true, LOG_LEVEL_DEBUG);
    // 1. Cargar configuracion
    config_kernel = read_config(config_path, logger_aux);
    if (config_kernel == NULL)
    {
        log_error(logger_main, "No se pudo cargar la configuracion");
        end_program(logger_main, logger_aux, config_kernel, modules_client, queues);
        return EXIT_FAILURE;
    }
    // 2. Crear clientes
    modules_client = start_modules_client(config_kernel, logger_aux);
    /* if (modules_client == NULL)
    {
        log_error(logger_main, "No se pudo crear los clientes");
        end_program(logger_main, logger_aux, config_kernel, modules_client, queues);
        return EXIT_FAILURE;
    } */
    // 3. Crear estructuras kernel
    queues = create_queues();
    // 4. Levantar hilo de servidor
    start_kernel_server(config_kernel->puerto_escucha);
    // 4.1. Levantar servidor - OK
    // 4.2. Recibir cliente - OK
    // 4.3. Crear hilo para cliente - OK
    // 4.4. Crear PCB - OK
    // 4.5. Solicitar Tabla de Segmentos - OK
    // 4.6. Alojar PCB en cola de ready - OK
    // 4.7. Inicializar mutex - OK
    inicializar_mutex();
    // 5. Levantar hilo de kernel
    start_kernel_core();
    // 5.1. Verificar contenido de colas - OK
    // 5.2. Aplicar algoritmo - OK
    // 5.3. Procesar CPU
    // 5.4. Actualizar estados - OK
    // 6. Mostrar info del sistema
    // Mantengo abierto el programa por ahora
    sleep(3);
    while (end_program_flag == false)
    {
        print_menu();
    }
    // 6. Cerrar servidor - OK
    end_kernel_server();
    end_kernel_core();
    // 7. Limpiar estructuras - OK
    end_program(logger_main, logger_aux, config_kernel, modules_client, queues);
    return EXIT_SUCCESS;
}

void sigintHandler(int signum)
{
    printf("\nIniciando fin del modulo por signal: %d\n", signum);
    end_kernel_server();
    end_kernel_core();
    end_program(logger_main, logger_aux, config_kernel, modules_client, queues);
    exit(signum);
}

// PRINTS FUNCTIONS

void print_menu()
{
    char *input;
    printf("\n\nMenu:\n");
    printf("1. Mostrar config\n");
    printf("2. Mostrar colas\n");
    printf("3. Mostrar estados internos\n");
    printf("4. Mostrar PCB\n");
    printf("5. Salir\n");
    input = readline("Ingrese una opcion: ");
    printf("\n\n");
    switch (atoi(input))
    {
    case 1:
        print_config();
        break;
    case 2:
        print_queues();
        break;
    case 3:
        print_internal_states();
        break;
    case 4:
        input = readline("Ingrese el PID del PCB: ");
        t_pcb *pcb = get_pcb_by_pid(atoi(input));
        printf("\n");
        if (pcb != NULL)
            print_pcb(pcb);
        else
            printf("No se encontro el PCB\n");
        break;
    case 5:
        end_program_flag = true;
        break;
    default:
        printf("Opcion invalida\n");
        break;
    }
    sleep(1);
    free(input);
}

t_pcb *get_pcb_by_pid(int pid)
{
    t_pcb *pcb;
    for (int i = 0; i < list_size(queues->NEW); i++)
    {
        pcb = list_get(queues->NEW, i);
        if (pcb->pid == pid)
            return pcb;
    }
    for (int i = 0; i < list_size(queues->READY); i++)
    {
        pcb = list_get(queues->READY, i);
        if (pcb->pid == pid)
            return pcb;
    }
    for (int i = 0; i < list_size(queues->EXEC); i++)
    {
        pcb = list_get(queues->EXEC, i);
        if (pcb->pid == pid)
            return pcb;
    }
    for (int i = 0; i < list_size(queues->BLOCK); i++)
    {
        pcb = list_get(queues->BLOCK, i);
        if (pcb->pid == pid)
            return pcb;
    }
    for (int i = 0; i < list_size(queues->EXIT); i++)
    {
        pcb = list_get(queues->EXIT, i);
        if (pcb->pid == pid)
            return pcb;
    }
    return NULL;
}

void print_config()
{
    printf("Configuracion:\n");
    printf("IP_MEMORIA: %s\n", config_kernel->ip_memoria);
    printf("PUERTO_MEMORIA: %s\n", config_kernel->puerto_memoria);
    printf("IP_FILESYSTEM: %s\n", config_kernel->ip_filesystem);
    printf("PUERTO_FILESYSTEM: %s\n", config_kernel->puerto_filesystem);
    printf("IP_CPU: %s\n", config_kernel->ip_cpu);
    printf("PUERTO_CPU: %s\n", config_kernel->puerto_cpu);
    printf("PUERTO_ESCUCHA: %s\n", config_kernel->puerto_escucha);
    printf("GRADO_MAX_MULTIPROGRAMACION: %d\n", config_kernel->grado_max_multiprog);
    printf("ALGORITMO_PLANIFICACION: %s\n", config_kernel->algoritmo_planificacion);
    printf("ESTIMACION_INICIAL: %f\n", config_kernel->estimacion_inicial);
    printf("HRRN_ALFA: %f\n", config_kernel->hrrn_alfa);
    list_iterate(config_kernel->recursos, (void *)print_recurso);
    list_iterate(config_kernel->instancias_recursos, (void *)print_instancias_recursos);
}

void print_recurso(char *recurso)
{
    printf("RECURSO: %s\n", recurso);
}

void print_instancias_recursos(char *instancias_recursos)
{
    printf("INSTANCIAS X RECURSO: %s\n", instancias_recursos);
}

void print_queues()
{
    printf("Cola de NEW:\n");
    list_iterate(queues->NEW, (void *)print_pcb_pid);
    printf("Cola de READY:\n");
    list_iterate(queues->READY, (void *)print_pcb_pid);
    printf("Cola de EXEC:\n");
    list_iterate(queues->EXEC, (void *)print_pcb_pid);
    printf("Cola de BLOCK:\n");
    list_iterate(queues->BLOCK, (void *)print_pcb_pid);
    printf("Cola de EXIT:\n");
    list_iterate(queues->EXIT, (void *)print_pcb_pid);
}

void print_pcb_pid(t_pcb *pcb)
{
    printf("  PID: %d\n", pcb->pid);
}

void print_pcb(t_pcb *pcb)
{
    printf("  PID: %d\n", pcb->pid);
    printf("  Program Counter: %d\n", pcb->program_counter);
    printf("  Cantidad de instrucciones: %d\n", list_size(pcb->instrucciones));
    // printf("  Cantidad de segmentos: %d\n", list_size(pcb->segments_table));
    // printf("  Cantidad de archivos abiertos: %d\n", list_size(pcb->open_files_table));
    int64_t time_ready = temporal_diff(temporal_create(), pcb->tiempo_llegada_ready);
    printf("  Tiempo de llegada a READY: %ld\n", time_ready);
    printf("  Estimacion de rafaga: %f\n", pcb->est_sig_rafaga);
    printf("  Registros:\n");
    print_registers(pcb->registers);
}

void print_registers(t_registers *registers)
{
    printf("      AX: ");
    for (int i = 0; i < 4; i++)
    {
        printf("%c", *(char *)(registers->AX + i));
    }
    printf("\n");
    printf("      BX: ");
    for (int i = 0; i < 4; i++)
    {
        printf("%c", *(char *)(registers->BX + i));
    }
    printf("\n");
    printf("      CX: ");
    for (int i = 0; i < 4; i++)
    {
        printf("%c", *(char *)(registers->CX + i));
    }
    printf("\n");
    printf("      DX: ");
    for (int i = 0; i < 4; i++)
    {
        printf("%c", *(char *)(registers->DX + i));
    }
    printf("\n");
    printf("      EAX: ");
    for (int i = 0; i < 8; i++)
    {
        printf("%c", *(char *)(registers->EAX + i));
    }
    printf("\n");
    printf("      EBX: ");
    for (int i = 0; i < 8; i++)
    {
        printf("%c", *(char *)(registers->EBX + i));
    }
    printf("\n");
    printf("      ECX: ");
    for (int i = 0; i < 8; i++)
    {
        printf("%c", *(char *)(registers->ECX + i));
    }
    printf("\n");
    printf("      EDX: ");
    for (int i = 0; i < 8; i++)
    {
        printf("%c", *(char *)(registers->EDX + i));
    }
    printf("\n");
    printf("      RAX: ");
    for (int i = 0; i < 16; i++)
    {
        printf("%c", *(char *)(registers->RAX + i));
    }
    printf("\n");
    printf("      RBX: ");
    for (int i = 0; i < 16; i++)
    {
        printf("%c", *(char *)(registers->RBX + i));
    }
    printf("\n");
    printf("      RCX: ");
    for (int i = 0; i < 16; i++)
    {
        printf("%c", *(char *)(registers->RCX + i));
    }
    printf("\n");
    printf("      RDX: ");
    for (int i = 0; i < 16; i++)
    {
        printf("%c", *(char *)(registers->RDX + i));
    }
    printf("\n");
}

void print_internal_states()
{
    int grado_multiprog = list_size(queues->READY) + list_size(queues->EXEC) + list_size(queues->BLOCK);
    printf("Proximo PID: %d\n", pid_counter);
    printf("Grado de multiprogramacion: %d\n", grado_multiprog);
    printf("Aceptar conexiones consola: %d\n", accept_connections);
    printf("Nucleo kernel estado: %d\n", core_running);
}
