#include "modulos.h"

// KERNEL
void kernel_operations(int client_socket)
{
    bool exit = false;
    while (!exit)
    {
        t_package *package = get_package(client_socket, logger_aux);
        switch (package->operation_code)
        {
        case COMPACTAR:
            compact_memory();
            log_info(logger_main, "RESULTADO DE LA COMPACTACION");
            print_all_segments_tables();
            send_ltsegmentos(client_socket, all_segments_tables, logger_aux);
            break;
        case PID_INSTRUCCION:
            t_pid_instruccion *pidtruction = get_pid_instruccion(package);
            handle_kernel_pid_instruction(client_socket, pidtruction);
            instruccion_destroy(pidtruction->instruccion);
            free(pidtruction);
            break;
        case PID_STATUS:
            t_pid_status *pid_status = get_pid_status(package);
            handle_pid_status(client_socket, pid_status);
            free(pid_status);
            break;
        case END:
            log_warning(logger_aux, "Kernel: Conexion Finalizada");
            exit = true;
            end_program_flag = true;
            break;
        default:
            log_warning(logger_aux, "Kernel: Operacion desconocida");
            exit = true;
            end_program_flag = true;
            break;
        }
        package_destroy(package);
    }
}

void handle_kernel_pid_instruction(int client_socket, t_pid_instruccion *pidtruction)
{
    t_instruccion *instruccion = pidtruction->instruccion;
    int pid = pidtruction->pid;
    int id = atoi((char *)list_get(instruccion->parametros, 0));
    switch (instruccion->identificador)
    {
    case I_CREATE_SEGMENT:
        int size = atoi((char *)list_get(instruccion->parametros, 1));
        t_segment *segment = create_segment(id, size);
        if (segment != NULL && segment->base_address != -1)
        {
            // SUCCESS
            add_segment_to_table(pid, segment);
            log_info(logger_main, "PID: %d - Crear Segmento: %d - Base: %d - Tamanio: %d", pid, id, segment->base_address, size);
            send_segment(client_socket, segment, logger_aux);
        }
        else
        {
            if (segment == NULL) // OUT OF MEMORY
                send_status_code(client_socket, OUT_OF_MEMORY, logger_aux);
            else
            {
                // COMPACTATION REQUIRED
                log_info(logger_main, "Solicitud de Compactacion");
                send_status_code(client_socket, COMPACTATION_REQUIRED, logger_aux);
            }
            free(segment);
        }
        break;
    case I_DELETE_SEGMENT:
        t_segment *sgmnt = get_segment_by_id(id);
        log_info(logger_main, "PID: %d - Eliminar Segmento: %d - Base: %d - Tamanio: %d", pid, id, sgmnt->base_address, sgmnt->size);
        delete_segment(pid, sgmnt);
        t_segments_table *segments_table = get_segments_table_by_pid(pid);
        send_tsegmento(client_socket, segments_table, logger_aux);
        break;
    default:
        log_warning(logger_aux, "Instruccion desconocida");
        break;
    }
    // HACER UN FREE DE PIDTRUCTION POR FAVOR
}

void handle_pid_status(int client_socket, t_pid_status *pid_status)
{
    switch (pid_status->status)
    {
    case PROCESS_NEW:
        t_segments_table *st = create_segments_table(pid_status->pid);
        log_info(logger_main, "Creacion de Proceso PID: %d", pid_status->pid);
        send_tsegmento(client_socket, st, logger_aux);
        break;
    case PROCESS_END:
        log_info(logger_main, "Eliminacion de Proceso PID: %d", pid_status->pid);
        delete_segments_table(get_segments_table_by_pid(pid_status->pid));
        break;
    default:
        break;
    }
}

// CPU
void cpu_operations(int client_socket)
{
    bool exit = false;
    while (!exit)
    {
        t_package *package = get_package(client_socket, logger_aux);
        switch (package->operation_code)
        {
        case PID_INSTRUCCION:
            t_pid_instruccion *pidtruction = get_pid_instruccion(package);
            handle_cpu_pid_instruction(client_socket, pidtruction);
            instruccion_destroy(pidtruction->instruccion);
            free(pidtruction);
        case END:
            log_info(logger_aux, "Conexion Finalizada");
            exit = true;
            break;
        default:
            log_warning(logger_aux, "Operacion desconocida\n");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}

void handle_cpu_pid_instruction(int client_socket, t_pid_instruccion *pidtruction)
{
    t_instruccion *instruction = pidtruction->instruccion;
    bool res = false;
    int base_address = 0;
    int length = 0;
    switch (instruction->identificador)
    {
    case I_MOV_IN: // lee de memoria y pasa valor leido
        base_address = atoi((char *)list_get(instruction->parametros, 0));
        length = atoi((char *)list_get(instruction->parametros, 1));
        // t_info *info = read_memory(base_address, length);
        // log_info(logger_main, "PID: %d - Acción: LEER - Dirección física: %d - Tamaño: %d - Origen: CPU ", pidtruction->pid, base_address, length);
        // res = send_info(client_socket, info, logger_aux);
        // if(!res)
        //     log_error(logger_aux, "No se pudo enviar el valor leido de memoria a CPU (MOV_IN)");
        break;
    case I_MOV_OUT: // escribe en memoria y pasa t_status_code->SUCCESS
        base_address = atoi((char *)list_get(instruction->parametros, 0));
        char *valor_a_escribir = (char *)list_get(instruction->parametros, 1);
        length = instruction->p_length[1] - 1; // -1 pq es un char* y no queremos escribir el '\0'
        bool result = write_memory(base_address, length, valor_a_escribir);
        if (!result)
        {
            log_error(logger_aux, "No se pudo escribir en memoria (MOV_OUT)");
            res = send_status_code(client_socket, SEGMENTATION_FAULT, logger_aux);
        }
        else
        {
            log_info(logger_main, "PID: %d - Acción: ESCRIBIR - Dirección física: %d - Tamaño: %d - Origen: CPU ", pidtruction->pid, base_address, length);
            res = send_status_code(client_socket, SUCCESS, logger_aux);
        }
        if (!res)
            log_error(logger_aux, "No se pudo enviar el OK a CPU (MOV_OUT)");
        break;
    default:
        log_warning(logger_aux, "Instruccion desconocida");
    }
}

// FS
void fs_operations(int client_socket)
{
    bool exit = false;
    while (exit == false)
    {
        t_package *package = get_package(client_socket, logger_aux);
        switch (package->operation_code)
        {
        case END:
            log_warning(logger_aux, "Filesystem: Conexion Finalizada");
            exit = true;
            break;
        default:
            log_warning(logger_aux, "Filesystem: Operacion desconocida");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}

/*
void fs_operations(int client_socket)
{
    bool exit = false;
    while (!exit)
    {
        t_package *package = get_package(client_socket, logger_aux);
        switch (package->operation_code)
        {
        case INFO_WRITE:
            t_info_write *info_write = get_info_write(package);
            bool res = write_memory(info_write->base_address, info_write->info->size, info_write->info->data);
            if(!res){
                log_error(logger_aux, "Direccion de escritura en memoria invalida");
                exit = true;
            }
            else{
                log_info(logger_main, "PID: %d - Acción: ESCRIBIR - Dirección física: %d - Tamaño: %d - Origen: FS ", info_write->pid, info_write->base_address, info_write->size);
                send_status_code(client_socket, SUCCESS, logger_aux);
            }
            //HACER UN FREE DE INFO_WRITE POR FAVOR
            break;
        case INFO_READ:
            t_info_read *info_read = get_info_read(package);
            t_info *info = read_memory(info_read->base_address, info_read->length);
            if(info->data == NULL){
                log_error(logger_aux, "Direccion de lectura en memoria invalida");
                exit = true;
            }
            else{
                log_info(logger_main, "PID: %d - Acción: LEER - Dirección física: %d - Tamaño: %d - Origen: FS ", info_read->pid, info_read->base_address, info_read->size);
                send_info(client_socket, info, logger_aux);
            }
            //HACER UN FREE DE INFO_READ POR FAVOR
            break;
        case END:
            log_info(logger_aux, "Conexion Finalizada");
            exit = true;
            break;
        default:
            log_warning(logger_aux, "Operacion desconocida");
            exit = true;
            break;
        }
        package_destroy(package);
    }
    free(module);
}
*/