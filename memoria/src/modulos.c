#include "modulos.h"

void kernel_operations(int client_socket, t_log* logger){
    //ASUMO QUE EL HANDSHAKE YA LO CONTESTE
    while(1){
        t_package* package = get_package(client_socket,logger);
        t_instruccion* instruccion = get_instruccion(package);
        switch (instruccion->identificador){
        case I_CREATE_SEGMENT:
        case I_DELETE_SEGMENT:
            t_segments_table* s_table = create_segments_table(0,0,0);
            send_tsegmento(client_socket,s_table,logger);
            break;
        case I_EXIT:
            log_debug(logger,"Conexion Kernel-Memoria finalizada");
            return;
        default:
            log_warning(logger,"Operacion desconocida");
            break;
        }
    }
}

void cpu_operations(int client_socket,t_log* logger){
    //ASUMO QUE EL HANDSHAKE YA LO CONTESTE
    while(1){
        t_package* package = get_package(client_socket,logger);
        t_instruccion* instruccion = get_instruccion(package);
        switch (instruccion->identificador){
        case I_F_READ:
            t_adress adress= 1;
            send_adress(client_socket,adress,logger);
            break;
        case I_F_WRITE:
            t_adress adress= 1;
            send_adress(client_socket,adress,logger);
            break;
        case I_EXIT:
            log_debug(logger,"Conexion CPU-Memoria finalizada");
            return;
        default:
            log_warning(logger,"Operacion desconocida");
            break;
        }
    }
}

void fs_operations(int client_socket,t_log* logger){
    //ASUMO QUE EL HANDSHAKE YA LO CONTESTE
    while(1){
        t_package* package = get_package(client_socket,logger);
        t_instruccion* instruccion = get_instruccion(package);
        switch (instruccion->identificador){
        case I_F_READ:
            t_data *data = create_data(instruccion->parametros[1],instruccion->parametros[2]);
            send_data(client_socket,data,logger);
            break;
        case I_F_WRITE:
            t_data *data;
            data->value="OK";
            data->value_length=2;
            send_data(client_socket,data,logger);
            break;
        case I_EXIT:
            log_debug(logger,"Conexion FS-Memoria finalizada");
            return;
        default:
            log_warning(logger,"Operacion desconocida");
            break;
        }
    }
}