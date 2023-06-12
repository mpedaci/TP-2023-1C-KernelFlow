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
    while (exit == false)
    {
        t_package *package = get_package(client_socket, logger_aux);
        switch (package->operation_code)
        {
        case INSTRUCCION:
            t_instruccion *instruccion = get_instruccion(package);
            t_data *data = malloc(sizeof(t_data));
            bool res;
            int direccion_fisica;
            switch (instruccion->identificador)
            {
            case I_MOV_IN: // lee de memoria y pasa valor leido
                direccion_fisica = atoi((char*)list_get(instruccion->parametros, 0));
                
                // falta la parte de leer de memoria TODO

                data->value = "valor_leido"; // no se si hay que hacer un malloc aca - CHECKEAR
                data->value_length = strlen(data->value);
                res = send_data(client_socket, data, logger_aux);
                if(!res)
                    log_error(logger_aux, "No se pudo enviar el valor leido de memoria a CPU (MOV_IN)");
                free(data);
                break;
            case I_MOV_OUT: // escribe en memoria y pasa OK
                direccion_fisica = atoi((char*)list_get(instruccion->parametros, 0));
                char *valor_a_escribir = (char*)list_get(instruccion->parametros, 1);
                
                // falta la parte de escribir en memoria TODO

                data->value = "OK";
                data->value_length = strlen(data->value);
                res = send_data(client_socket, data, logger_aux);
                if(!res)
                    log_error(logger_aux, "No se pudo enviar el OK a CPU (MOV_OUT)");
                free(data);
                break;
            default:
                printf("Instruccion desconocida\n");
                exit = true;
                break;
            }
            instruction_destroyer(instruccion);
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