#include "filesystem_server.h"

void start_filesystem_server()
{
    int server_socket = server_start(config_fs->puerto_escucha, logger_aux);
    int client_socket;
    bool accept_connection = true;
    while (accept_connection)
    {
        client_socket = client_wait(server_socket, logger_aux);
        if (client_socket != -1)
        {
            log_info(logger_aux, "Se conecto un cliente");

            if (hs_server_to_module_valid(client_socket, HSFS, logger_aux))
                process_client(client_socket);

            log_info(logger_aux, "Cerrando conexion con cliente");
            socket_destroy(client_socket);
            socket_destroy(server_socket);
            accept_connection = false;
        }
    }
}

void process_client(int client_socket)
{
    bool exit = false;
    while (exit == false)
    {
        t_package *package = get_package(client_socket, logger_aux);
        switch (package->operation_code)
        {
        case INSTRUCCION:
            t_instruccion *instruccion = get_instruccion(package);
            handle_instruccion(instruccion, client_socket);
            break;
        case END:
            log_warning(logger_aux, "Conexion Finalizada");
            exit = true;
            break;
        default:
            log_warning(logger_aux, "Operacion desconocida.");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}

void handle_instruccion(t_instruccion *instruccion, int client_socket)
{
    switch (instruccion->identificador)
    {
    case I_F_OPEN:
        log_info(logger_aux, "Se recibio una instruccion de abrir archivo");
        char *nombre = (char*)list_get(instruccion->parametros, 0);
        if (instruccion->cant_parametros > 1)
        {
            if (create_file(nombre))
                send_status_code(client_socket, FILE_CREATED, logger_aux);
            else
                send_status_code(client_socket, FILE_OPEN, logger_aux);
        } else {
            if (open_file(nombre))
                send_status_code(client_socket, FILE_OPEN, logger_aux);
            else
                send_status_code(client_socket, FILE_NOT_EXIST, logger_aux);
        }
        
        instruccion_destroy(instruccion);
        break;
    case I_F_READ:
    case I_F_WRITE:
    case I_F_TRUNCATE:
        log_info(logger_aux, "Se recibio una instruccion de leer/escribir/truncar archivo");
        t_inst_client *ic = malloc(sizeof(t_inst_client));
        ic->instruccion = instruccion;
        ic->client_socket = client_socket;
        process_request_async(ic);
        break;
    default:
        log_warning(logger_aux, "Instruccion desconocida.");
        send_status_code(client_socket, ERROR, logger_aux);
        break;
    }
}

void process_request_async(t_inst_client *icr)
{
    t_instruccion *instruccion = icr->instruccion;
    int client_socket = icr->client_socket;
    char *nombre = (char *)list_get(instruccion->parametros, 0);
    int puntero_archivo = -1;
    int cant_bytes = -1;
    int direccion_fisica = -1;
    switch (instruccion->identificador)
    {
    case I_F_READ:
        log_info(logger_aux, "Se recibio una instruccion de leer archivo");
        direccion_fisica = atoi((char*)list_get(instruccion->parametros, 1));
        cant_bytes = atoi((char*)list_get(instruccion->parametros, 2));
        puntero_archivo = atoi((char*)list_get(instruccion->parametros, 3));
        if (read_file(nombre, puntero_archivo, cant_bytes, direccion_fisica))
            send_status_code(client_socket, FILE_READ, logger_aux);
        else
            send_status_code(client_socket, FILE_NOT_EXISTS, logger_aux);
        break;
    case I_F_WRITE:
        log_info(logger_aux, "Se recibio una instruccion de escribir archivo");
        direccion_fisica = atoi((char*)list_get(instruccion->parametros, 1));
        cant_bytes = atoi((char*)list_get(instruccion->parametros, 2));
        puntero_archivo = atoi((char*)list_get(instruccion->parametros, 3));
        if (write_file(nombre, puntero_archivo, cant_bytes, direccion_fisica))
            send_status_code(client_socket, FILE_WRITTEN, logger_aux);
        else
            send_status_code(client_socket, ERROR, logger_aux);
        break;
    case I_F_TRUNCATE:
        log_info(logger_aux, "Se recibio una instruccion de truncar archivo");
        int nuevo_tamanio = atoi((char*)list_get(instruccion->parametros, 1));
        if (truncate_file(nombre, nuevo_tamanio))
            send_status_code(client_socket, FILE_TRUNCATED, logger_aux);
        else 
            send_status_code(client_socket, FILE_NOT_EXISTS, logger_aux);
        break;
    default:
        break;
    }
    instruccion_destroy(instruccion);
    free(icr);
}