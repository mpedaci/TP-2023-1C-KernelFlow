#include "filesystem_server.h"

void start_filesystem_server(char *listen_port, t_log *logger)
{
    int server_socket = server_start(listen_port, logger);
    int client_socket;
    bool accept_connection = true;
    while (accept_connection)
    {
        client_socket = client_wait(server_socket, logger);
        if (client_socket != -1)
        {
            log_info(logger, "Se conecto un cliente");

            if (hs_server_to_module_valid(client_socket, HSFS, logger))
                process_client(client_socket, logger);

            log_info(logger, "Cerrando conexion con cliente");
            socket_destroy(client_socket);
            socket_destroy(server_socket);
            accept_connection = false;
        }
    }
}

void process_client(int client_socket, t_log *logger)
{
    bool exit = false;
    while (exit == false)
    {
        t_package *package = get_package(client_socket, logger);
        switch (package->operation_code)
        {
        case INSTRUCCION:
            log_debug(logger, "RECIBIDA INSTRUCCION");
            t_instruccion *instruccion = get_instruccion(package);
            handle_instruccion(instruccion, client_socket, logger);
            list_destroy_and_destroy_elements(instruccion->parametros, free);
            free(instruccion);
            break;
        case END:
            log_warning(logger, "Conexion Finalizada");
            exit = true;
            break;
        default:
            log_warning(logger, "Operacion desconocida.");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}




void handle_instruccion(t_instruccion *instruccion, int client_socket, t_log *logger)
{
    /*
    int timeout = 2000;
    sleep(timeout / 1000);
    int response; 
    switch (instruccion->identificador)
    {
    case I_F_OPEN:
        log_info(logger, "Se recibio una instruccion de abrir archivo");
        response= open_file(config,nombre,logger);
        if(response==0){
        send_status_code(client_socket, FILE_OPEN, logger);
        }else{
        //response= create_file(config,nombre,logger);
        send_status_code(client_socket, ERROR, logger);
        }
        break;
    case I_F_READ:
        log_info(logger, "Se recibio una instruccion de leer archivo");
        response= read_file(config,puntero_archivo,nombre,cant_bytes,direccion_fisica,logger);
        if(response==0){
         send_status_code(client_socket, FILE_READ, logger);
        }else{
        send_status_code(client_socket, ERROR, logger);
        }
        break;
    case I_F_WRITE:
        log_info(logger, "Se recibio una instruccion de escribir archivo");
        response= write_file(config,puntero_archivo,nombre,cant_bytes,direccion_fisica,logger);
        if(response==0){
        send_status_code(client_socket, FILE_WRITTEN, logger);
        }else{
        send_status_code(client_socket, ERROR, logger);
        }
        break;
    case I_F_TRUNCATE:
        log_info(logger, "Se recibio una instruccion de truncar archivo");

        response= open_file(config,nombre,logger);
        if(response ==0){
            response= truncate_file(config,nuevo_tamanio,nombre,logger);
            send_status_code(client_socket, FILE_TRUNCATED, logger);
        }else{
            response= create_file(config,nombre,logger);
            response= truncate_file(config,nuevo_tamanio,nombre,logger);
            send_status_code(client_socket, FILE_TRUNCATED, logger);
        }
        break;
    default:
        log_warning(logger, "Instruccion desconocida.");
        send_status_code(client_socket, ERROR, logger);
        break;
    }
    */
}

