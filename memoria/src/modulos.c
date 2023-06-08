#include "modulos.h"

void kernel_operations(int client_socket){
    //ASUMO QUE EL HANDSHAKE YA LO CONTESTE
    // CHEQUEAR
    /* while(1){
        t_package* package = get_package(client_socket,logger_aux);
        t_instruccion* instruccion = get_instruccion(package);
        //Esto es una ensalada de frutas
        t_segment* segment = create_segment(0,0,0);
        t_list* s_table= list_create();
        list_add(s_table,segment);
        switch (instruccion->identificador){
        case I_CREATE_SEGMENT:
        case I_DELETE_SEGMENT:
            send_tsegmento(client_socket,s_table,logger_aux);
            break;
        case I_EXIT:
            log_debug(logger_aux,"Conexion Kernel-Memoria finalizada");
            return;
        default:
            log_warning(logger_aux,"Operacion desconocida");
            break;
        }
    } */
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

void cpu_operations(int client_socket){
    //ASUMO QUE EL HANDSHAKE YA LO CONTESTE
    /* while(1){
        t_package* package = get_package(client_socket,logger_aux);
        t_instruccion* instruccion = get_instruccion(package);
        t_address address;
        switch (instruccion->identificador){
        case I_MOV_IN:
            address= 1;
            send_address(client_socket,address,logger_aux);
            break;
        case I_MOV_OUT:
            address= 1;
            send_address(client_socket,address,logger_aux);
            break;
        case I_EXIT:
            log_debug(logger_aux,"Conexion CPU-Memoria finalizada");
            return;
        default:
            log_warning(logger_aux,"Operacion desconocida");
            break;
        }
    } */
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

void fs_operations(int client_socket){
    //ASUMO QUE EL HANDSHAKE YA LO CONTESTE
    /* while(1){
        t_package* package = get_package(client_socket,logger_aux);
        t_instruccion* instruccion = get_instruccion(package);
        t_data* data;
        switch (instruccion->identificador){
        case I_F_READ:
            data = create_data((char*)instruccion->parametros[1],instruccion->p1_length);
            send_data(client_socket,data,logger_aux);
            break;
        case I_F_WRITE:
            data->value="OK";
            data->value_length=2;
            send_data(client_socket,data,logger_aux);
            break;
        case I_EXIT:
            log_debug(logger_aux,"Conexion FS-Memoria finalizada");
            return;
        default:
            log_warning(logger_aux,"Operacion desconocida");
            break;
        }
    } */
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