#include "modulos.h"

void kernel_operations(int client_socket){
    //ASUMO QUE EL HANDSHAKE YA LO CONTESTE
    while(1){
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
    }
}

void cpu_operations(int client_socket){
    //ASUMO QUE EL HANDSHAKE YA LO CONTESTE
    while(1){
        t_package* package = get_package(client_socket,logger_aux);
        t_instruccion* instruccion = get_instruccion(package);
        t_adress adress;
        switch (instruccion->identificador){
        case I_MOV_IN:
            adress= 1;
            send_adress(client_socket,adress,logger_aux);
            break;
        case I_MOV_OUT:
            adress= 1;
            send_adress(client_socket,adress,logger_aux);
            break;
        case I_EXIT:
            log_debug(logger_aux,"Conexion CPU-Memoria finalizada");
            return;
        default:
            log_warning(logger_aux,"Operacion desconocida");
            break;
        }
    }
}

void fs_operations(int client_socket){
    //ASUMO QUE EL HANDSHAKE YA LO CONTESTE
    while(1){
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
    }
}