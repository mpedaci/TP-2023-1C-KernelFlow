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
            log_info(logger_main, "Solicitud de Compactacion");
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
        t_segments_table *pid_segments_table = get_segments_table_by_pid(pid);
        if (list_size(pid_segments_table->segment_list) >= config->segment_quantity)
        {
            send_status_code(client_socket, MAX_SEG_QUANTITY_REACHED, logger_aux);
            log_warning(logger_main, "PID: %d - Crear Segmento - No se puede crear mas segmentos", pid);
        }
        else
        {
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
                    log_info(logger_main, "Se envio una solicitud de compactacion");
                    send_status_code(client_socket, COMPACTATION_REQUIRED, logger_aux);
                }
                free(segment);
            }
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

void cpu_fs_operations(int client_socket, char *modulo)
{
    bool exit = false;
    while (!exit)
    {
        t_package *package = get_package(client_socket, logger_aux);
        bool res = false;
        int pid = -1;
        switch (package->operation_code)
        {
        case INFO_READ:
            t_info_read *info_read = get_info_read(package);
            t_info *info = read_memory(info_read->base_address, info_read->size);
            pid = get_pid_by_address(info_read->base_address);
            if (pid == -1)
            {
                log_error(logger_aux, "No se encontro un segmento para esa direccion fisica"); // CHEQUEAR ERROR HANDLE
                send_status_code(client_socket, SEGMENTATION_FAULT, logger_aux);
            }
            else
            {
                log_info(logger_main, "PID: %d - Accion: LEER - Direccion fisica: %d - Tamanio: %d - Origen: %s ", pid, info_read->base_address, info_read->size, modulo);
                char *vR = malloc(info_read->size + 1);
                *(vR + info_read->size) = '\0';
                memcpy(vR, info->data, info_read->size);
                log_info(logger_aux, "Valor leido: %s", vR);
                free(vR);
                res = send_info(client_socket, info, logger_aux);
                if (!res)
                    log_error(logger_aux, "No se pudo enviar el valor leido de memoria a %s", modulo);
                free(info_read);
            }
            info_destroy(info);
            break;
        case INFO_WRITE:
            t_info_write *info_write = get_info_write(package);
            pid = get_pid_by_address(info_write->base_address);
            if (pid == -1)
            {
                log_error(logger_aux, "No se encontro un segmento para esa direccion fisica"); // CHEQUEAR ERROR HANDLE
                send_status_code(client_socket, SEGMENTATION_FAULT, logger_aux);
            }
            else {
                bool result = write_memory(info_write->base_address, info_write->info->size, info_write->info->data);
                if (!result)
                {
                    log_error(logger_aux, "No se pudo escribir en memoria");
                    res = send_status_code(client_socket, SEGMENTATION_FAULT, logger_aux);
                }
                else
                {
                    log_info(logger_main, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamanio: %d - Origen: %s ", pid, info_write->base_address, info_write->info->size, modulo);
                    char *vW = malloc(info_write->info->size + 1);
                    *(vW + info_write->info->size) = '\0';
                    memcpy(vW, info_write->info->data, info_write->info->size);
                    log_info(logger_aux, "Valor escrito: %s", vW);
                    free(vW);

                    res = send_status_code(client_socket, SUCCESS, logger_aux);
                }
                if (!res)
                    log_error(logger_aux, "No se pudo enviar el OK a %s", modulo);
            }
            info_write_destroy(info_write);
            break;
        case END:
            log_warning(logger_aux, "%s: Conexion Finalizada", modulo);
            exit = true;
            break;
        default:
            log_warning(logger_aux, "%s: Operacion desconocida", modulo);
            exit = true;
            break;
        }
        package_destroy(package);
    }
}