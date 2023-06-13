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
            handle_pid_instruction(client_socket, pidtruction);
            break;
        case END:
            log_info(logger_aux, "Conexion Finalizada");
            exit = true;
            accept_connections = false;
            break;
        default:
            log_warning(logger_aux, "Operacion desconocida");
            exit = true;
            accept_connections = false;
            break;
        }
        package_destroy(package);
    }
}

void handle_pid_instruction(int client_socket, t_pid_instruccion *pidtruction)
{
    t_instruccion *instruccion = pidtruction->instruccion;
    int pid = pidtruction->pid;
    int id = atoi((char *)list_get(instruccion->parametros, 0));
    switch (instruccion->identificador)
    {
    case I_CREATE_SEGMENT:
        int size = atoi((char *)list_get(instruccion->parametros, 1));
        t_segment *segment = create_segment(id, size);
        if (segment == NULL)
        {
            // OUT OF MEMORY
            send_status_code(client_socket, OUT_OF_MEMORY, logger_aux);
            break;
        }
        else if (segment->base_address == -1)
        {
            // COMPACTATION REQUIRED
            log_info(logger_main, "Solicitud de Compactacion");
            send_status_code(client_socket, COMPACTATION_REQUIRED, logger_aux);
            break;
        }
        else
        {
            // SUCCESS
            add_segment_to_table(pid, segment);
            log_info(logger_main, "PID: %d - Crear Segmento: %d - Base: %d - Tamanio: %d", pid, id, segment->base_address, size);
            send_segment(client_socket, segment, logger_aux);
        }
        break;
    case I_DELETE_SEGMENT:
        t_segment *sgmnt = get_segment_by_id(id);
        log_info(logger_main, "PID: %d - Eliminar Segmento: %d - Base: %d - Tamanio: %d", pid, id, sgmnt->base_address, sgmnt->size);
        delete_segment(pid, sgmnt);
        t_segments_table *segments_table = get_segments_table_by_pid(pid);
        send_tsegmento(client_socket, segments_table, logger_aux);
        break;
    case I_EXIT:
        log_info(logger_main, "PID: %d - Finalizar Proceso", pid);
        delete_segments_table(get_segments_table_by_pid(pid));
        send_status_code(client_socket, SUCCESS, logger_aux);
        break;
    default:
        log_warning(logger_aux, "Instruccion desconocida");
        break;
    }
    // HACER UN FREE DE PIDTRUCTION POR FAVOR
}

// CPU
void cpu_operations(int client_socket)
{
    bool exit = false;
    while (exit == false)
    {
        t_package *package = get_package(client_socket, logger_aux);
        switch (package->operation_code)
        {
        case INSTRUCCION:
            t_instruccion *instruccion = get_instruccion(package);
            t_address address;
            switch (instruccion->identificador)
            {
            case I_MOV_IN:
                address = 1;
                send_address(client_socket, address, logger_aux);
                break;
            case I_MOV_OUT:
                address = 1;
                send_address(client_socket, address, logger_aux);
                break;
            default:
                log_warning(logger_aux, "Instruccion desconocida");
                exit = true;
                break;
            }
            // hay que hacer un free de la intruccion :)
        case END:
            log_warning(logger_aux, "Conexion Finalizada");
            exit = true;
            break;
        default:
            log_warning(logger_aux, "Operacion desconocida");
            exit = true;
            break;
        }
        package_destroy(package);
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
            log_warning(logger_aux, "Conexion Finalizada");
            exit = true;
            break;
        default:
            log_warning(logger_aux, "Operacion desconocida");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}

/*
void cpu_and_fs_operations(int client_socket,char *module)
{
    bool exit = false;
    while (exit == false)
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
                log_info(logger_main, "PID: %d - Acción: ESCRIBIR - Dirección física: %d - Tamaño: %d - Origen: %s ", info_write->pid, info_write->base_address, info_write->size, module);
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
                log_info(logger_main, "PID: %d - Acción: LEER - Dirección física: %d - Tamaño: %d - Origen: %s ", info_read->pid, info_read->base_address, info_read->size, module);
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