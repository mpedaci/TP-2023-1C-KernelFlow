#include "communication.h"

/* OPERACIONES DE ENVIO DE INFORMACION */

/* CLIENTE -> SERVIDOR -> PROGRAMA */

t_package *get_package(int socket, t_log *logger)
{
    t_package *paquete = package_recv(socket, logger);
    return paquete;
};

t_list *get_instrucciones(t_package *paquete)
{
    t_list *instrucciones = t_lista_instrucciones_create_from_buffer(paquete->buffer);
    return instrucciones;
};

t_pcontexto *get_pcontexto(t_package *paquete)
{
    t_pcontexto *contexto = t_pcontexto_create_from_buffer(paquete->buffer);
    return contexto;
};

t_pcontexto_desalojo *get_pcontexto_desalojo(t_package *paquete)
{
    t_pcontexto_desalojo *contexto = t_pcontexto_desalojo_create_from_buffer(paquete->buffer);
    return contexto;
};

t_open_files *get_ofile(t_package *paquete)
{
    t_open_files *ofiles = t_open_files_create_from_buffer(paquete->buffer);
    return ofiles;
};

t_data* get_data(t_package* paquete)
{
    t_data* data = t_data_create_from_buffer(paquete->buffer);
    return data;
};

t_address get_address(t_package* paquete)
{
    t_address address = t_address_create_from_buffer(paquete->buffer);
    return address;
};

t_instruccion *get_instruccion(t_package *paquete)
{
    uint32_t offset = 0;
    t_instruccion *instruccion = t_instruccion_create_from_buffer(paquete->buffer, &offset);
    return instruccion;
};

t_list *get_ltsegmentos(t_package *paquete)
{
    t_list *lt_segmentos = t_lista_t_segments_create_from_buffer(paquete->buffer);
    return lt_segmentos;
};

t_segments_table *get_tsegmento(t_package *paquete)
{
    uint32_t offset = 0;
    t_segments_table *segments_table = t_segments_table_create_from_buffer(paquete->buffer, &offset);
    return segments_table;
};

t_segment *get_segment(t_package *paquete) {
    uint32_t offset = 0;
    t_segment *segment = t_segment_create_from_buffer(paquete->buffer, &offset);
    return segment;
};

t_status_code get_status_code(t_package *paquete) {
    t_status_code status_code = t_status_code_create_from_buffer(paquete->buffer);
    return status_code;
};

t_pid_instruccion *get_pid_instruccion(t_package *paquete) {
    t_pid_instruccion *pid_instruccion = t_pid_instruccion_create_from_buffer(paquete->buffer);
    return pid_instruccion;
};

t_pid_status *get_pid_status(t_package *paquete) {
    t_pid_status *pid_status = t_pid_status_create_from_buffer(paquete->buffer);
    return pid_status;
}

t_info_write *get_info_write(t_package* paquete) {
    t_info_write *info_write = t_info_write_create_from_buffer(paquete->buffer);
    return info_write;
}

t_info_read *get_info_read(t_package* paquete) {
    t_info_read *info_read = t_info_read_create_from_buffer(paquete->buffer);
    return info_read;
}

t_info *get_info(t_package* paquete) {
    t_info *info = t_info_create_from_buffer(paquete->buffer);
    return info;
}

/* PROGRAMA -> CLIENTE -> SERVIDOR */

bool send_instrucciones(int socket, t_list *lista_instrucciones, t_log *logger)
{
    t_buffer *buffer = t_lista_instrucciones_create_buffer(lista_instrucciones);
    t_package *paquete = package_create(buffer, INSTRUCCIONES);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_pcontexto(int socket, t_pcontexto *contexto, t_log *logger)
{
    t_buffer *buffer = t_pcontexto_create_buffer(contexto);
    t_package *paquete = package_create(buffer, PCONTEXTO);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_pcontexto_desalojo(int socket, t_pcontexto_desalojo *contexto, t_log *logger)
{
    t_buffer *buffer = t_pcontexto_desalojo_create_buffer(contexto);
    t_package *paquete = package_create(buffer, PCONTEXTODESALOJO);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_ofile(int socket, t_open_files *t_ofiles, t_log *logger)
{
    t_buffer *buffer = t_open_files_create_buffer(t_ofiles);
    t_package *paquete = package_create(buffer, OFILE);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_data(int socket, t_data* data, t_log *logger)
{
    t_buffer *buffer = t_data_create_buffer(data);
    t_package *paquete = package_create(buffer, DATA);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_address(int socket,t_address address, t_log* logger)
{
    t_buffer *buffer = t_address_create_buffer(address);
    t_package *paquete = package_create(buffer, FILEADDRESS);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
}

bool send_instruccion(int socket, t_instruccion *instruccion, t_log *logger)
{
    t_buffer *buffer = t_instruccion_create_buffer(instruccion);
    t_package *paquete = package_create(buffer, INSTRUCCION);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_end(int socket, t_log *logger)
{
    t_package *paquete = package_create(NULL, END);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_exit(int socket, t_log *logger)
{
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    instruccion->identificador = I_EXIT;
    instruccion->cant_parametros = 0;
    instruccion->parametros = list_create();
    for (size_t i = 0; i < 4; i++)
        instruccion->p_length[i] = 0;
    t_buffer *buffer = t_instruccion_create_buffer(instruccion);
    t_package *paquete = package_create(buffer, INSTRUCCION);
    bool res = package_send(socket, paquete, logger);
    list_destroy(instruccion->parametros);
    free(instruccion);
    package_destroy(paquete);
    return res;
};

bool send_ltsegmentos(int socket, t_list *lt_segmentos, t_log *logger)
{
    t_buffer *buffer = t_lista_t_segments_create_buffer(lt_segmentos);
    t_package *paquete = package_create(buffer, LTSEGMENTOS);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_tsegmento(int socket, t_segments_table *t_segmento, t_log *logger)
{
    t_buffer *buffer = t_segments_table_create_buffer(t_segmento);
    t_package *paquete = package_create(buffer, TSEGMENTOS);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_segment(int socket, t_segment *segmento, t_log *logger)
{
    t_buffer *buffer = t_segment_create_buffer(segmento);
    t_package *paquete = package_create(buffer, SEGMENTO);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_status_code(int socket, t_status_code status_code, t_log *logger)
{
    t_buffer *buffer = t_status_code_create_buffer(status_code);
    t_package *paquete = package_create(buffer, STATUS_CODE);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_pid_instruccion(int socket, t_pid_instruccion *pid_instruccion, t_log *logger)
{
    t_buffer *buffer = t_pid_instruccion_create_buffer(pid_instruccion);
    t_package *paquete = package_create(buffer, PID_INSTRUCCION);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_compactar(int socket, t_log *logger)
{
    t_package *paquete = package_create(NULL, COMPACTAR);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
};

bool send_pid_status(int socket, t_pid_status *pid_status, t_log *logger)
{
    t_buffer *buffer = t_pid_status_create_buffer(pid_status);
    t_package *paquete = package_create(buffer, PID_STATUS);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
}

bool send_info_write(int socket, t_info_write *info_write, t_log *logger)
{
    t_buffer *buffer = t_info_write_create_buffer(info_write);
    t_package *paquete = package_create(buffer, INFO_WRITE);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
}

bool send_info_read(int socket, t_info_read *info_read, t_log *logger)
{
    t_buffer *buffer = t_info_read_create_buffer(info_read);
    t_package *paquete = package_create(buffer, INFO_READ);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
}

bool send_info(int socket, t_info *info, t_log *logger)
{
    t_buffer *buffer = t_info_create_buffer(info);
    t_package *paquete = package_create(buffer, INFO);
    bool res = package_send(socket, paquete, logger);
    package_destroy(paquete);
    return res;
}

/* HANDSHAKE */

bool hs_server_to_module_valid(int socket_client, hs_code hs_module_origin, t_log *logger)
{
    int HS_Recv_A = -1;
    int HS_Recv_B = -1;
    int HS_Origin = hs_module_origin;
    // Recibo conexion
    recv(socket_client, &HS_Recv_A, sizeof(int), MSG_WAITALL);
    // Mando quien soy
    send(socket_client, &HS_Origin, sizeof(int), 0);
    // Espero confirmacion
    recv(socket_client, &HS_Recv_B, sizeof(int), MSG_WAITALL);
    // Chequeo si es el modulo se desconecta
    if (HS_Recv_B == HSOK)
    {
        log_info(logger, "Handshake valido - Conexion aceptada");
        return true;
    }
    else
    {
        log_warning(logger, "Handshake invalido - Conexion rechazada");
        return false;
    }
}

int hs_server_to_module_get_type(int socket_client, hs_code hs_module_origin, t_log *logger)
{
    int HS_Recv_A = -1;
    int HS_Recv_B = -1;
    int HS_Origin = hs_module_origin;
    // Recibo conexion
    recv(socket_client, &HS_Recv_A, sizeof(int), MSG_WAITALL);
    // Mando quien soy
    send(socket_client, &HS_Origin, sizeof(int), 0);
    // Espero confirmacion
    recv(socket_client, &HS_Recv_B, sizeof(int), MSG_WAITALL);
    // Chequeo si es el modulo se desconecta
    if (HS_Recv_B == HSOK)
    {
        log_info(logger, "Handshake valido - Conexion aceptada");
        return HS_Recv_A;
    }
    else
    {
        log_warning(logger, "Handshake invalido - Conexion rechazada");
        return -1;
    }
}

bool hs_client_to_module_valid(int socket_server, hs_code hs_module_origin, hs_code hs_module_expected, t_log *logger)
{
    int HS_Recv_A = -1;
    int HS_Send_A = -1;
    int HS_Origin = hs_module_origin;
    int HS_Expected = hs_module_expected;
    // Mando quien soy
    send(socket_server, &HS_Origin, sizeof(int), 0);
    // Recibo quien es el otro
    recv(socket_server, &HS_Recv_A, sizeof(int), MSG_WAITALL);
    // Chequeo si es el modulo que espero
    if (HS_Recv_A == HS_Expected)
    {
        log_info(logger, "Handshake valido - Conexion aceptada");
        HS_Send_A = HSOK;
    }
    else
    {
        log_warning(logger, "Handshake invalido - Conexion rechazada");
        HS_Send_A = HSFAIL;
    }
    send(socket_server, &HS_Send_A, sizeof(int), 0);
    return (HS_Send_A == HSOK);
}

/* Cliente - Servidor */

void socket_destroy(int socket)
{
    close(socket);
}