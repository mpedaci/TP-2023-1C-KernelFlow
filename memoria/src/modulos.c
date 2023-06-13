#include "modulos.h"

//KERNEL
void kernel_operations(int client_socket)
{
    bool exit = false;
    while (exit == false)
    {
        t_package *package = get_package(client_socket, logger_aux);
        switch (package->operation_code)
        {
        case PID_INSTRUCCION:
            t_pid_instruccion *pidtruction = get_pid_instruccion(package);
            handle_pid_instruction(client_socket, pidtruction);
            break;
        case COMPACTAR:
            compact_memory();
            send_ltsegmentos(client_socket,all_segments_tables,logger_aux);
            break;
        case END:
            printf("Conexion Finalizada\n");
            exit = true;
            break;
        default:
            printf("Operacion desconocida\n");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}

void handle_pid_instruction(int client_socket, t_pid_instruccion *pidtruction)
{
    t_instruccion *instruccion = pidtruction->instruccion;
    int pid = pidtruction->pid;
    int id = atoi((char*)list_get(instruccion->parametros, 0));
    switch (instruccion->identificador)
    {
    case I_CREATE_SEGMENT:
        int size = atoi((char*)list_get(instruccion->parametros, 1));
        t_segment *segment = create_segment(id,size);
        // OUT OF MEMORY
        if (segment == NULL)
        {
            send_status_code(client_socket, OUT_OF_MEMORY, logger_aux);
            break;
        } else if (segment->base_address == -1)
        // COMPACTATION REQUIRED
        {
            log_debug(logger_aux, "No se pudo crear el segmento, se necesita compactar");
            send_status_code(client_socket, COMPACTATION_REQUIRED, logger_aux);
            break;
        }
        // SUCCESS
        add_segment_to_table(pid,segment);
        //Esto no me gusta (Para mi deberia ser un unico paquete)
        send_status_code(client_socket, SUCCESS, logger_aux);
        send_segment(client_socket, segment, logger_aux);
        break;
    case I_DELETE_SEGMENT:
        delete_segment(pid, id);
        t_segments_table *segments_table = get_segments_table_by_pid(pid);
        send_tsegmento(client_socket, segments_table, logger_aux);
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
    while (!exit)
    {
        t_package *package = get_package(client_socket, logger_aux);
        switch (package->operation_code)
        {
        case INSTRUCCION:
            t_instruccion *instruction = get_instruccion(package);
            bool res;
            int base_address;
            int length;
            switch (instruction->identificador)
            {
            case I_MOV_IN: // lee de memoria y pasa valor leido
                base_address = atoi((char*)list_get(instruction->parametros, 0));
                length = atoi((char*)list_get(instruction->parametros, 1));

                t_info *info = read_memory(base_address, length);

                res = send_info(client_socket, info, logger_aux);
                if(!res)
                    log_error(logger_aux, "No se pudo enviar el valor leido de memoria a CPU (MOV_IN)");
                //HACER UN FREE DE INFO POR FAVOR
                break;
            case I_MOV_OUT: // escribe en memoria y pasa OK
                base_address = atoi((char*)list_get(instruction->parametros, 0));
                char *valor_a_escribir = (char*)list_get(instruction->parametros, 1);
                length = instruction->p_length[1];
                
                bool result = write_memory(base_address, length, valor_a_escribir);
                if(!result)
                    log_error(logger_aux, "No se pudo escribir en memoria (MOV_OUT)");

                res = send_status_code(client_socket, SUCCESS, logger_aux);
                if(!res)
                    log_error(logger_aux, "No se pudo enviar el OK a CPU (MOV_OUT)");
                break;
            default:
                log_warning(logger_aux, "Instruccion desconocida");
                exit = true;
                break;
            }
            //HACER UN FREE DE INSTRUCCION POR FAVOR
        case END:
            log_info(logger_aux,"Conexion Finalizada");
            exit = true;
            break;
        default:
            log_warning(logger_aux,"Operacion desconocida\n");
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
            printf("Conexion Finalizada\n");
            exit = true;
            break;
        default:
            printf("Operacion desconocida\n");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}

void instruction_destroyer(t_instruccion *instruccion)
{
    list_destroy_and_destroy_elements(instruccion->parametros, free);
    free(instruccion);
}