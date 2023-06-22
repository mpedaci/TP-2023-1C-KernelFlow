#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>     // send - recv
#include <unistd.h>         // close
#include <string.h>

#include <commons/log.h>

#include "types.h"
#include "package.h"

/* OPERACIONES DE ENVIO DE INFORMACION */

typedef enum
{
    INSTRUCCIONES,          // Pseudocodigo
    PCONTEXTO,              // Contexto de proceso
    PCONTEXTODESALOJO,      // Contexto de proceso desalojo
    LTSEGMENTOS,            // Lista de Tabla de Segmentos
    TSEGMENTOS,             // Tabla de Segmentos
    SEGMENTO,               // Segmento
    OFILE,                  // Archivo abierto
    FILEADDRESS,            // Datos Archivo
    DATA,                   // Datos
    INSTRUCCION,            // Instruccion
    END,                    // Fin Conexion - Programa
    STATUS_CODE,            // Codigo de estado
    PID_INSTRUCCION,        // Pid e instruccion
    COMPACTAR,              // Compactar
    PID_STATUS,             // Pid e status code
    INFO_WRITE,             // Pedido escritura a memoria
    INFO_READ,              // Pedido lectura a memoria
    INFO                    // Datos void*
} op_code;

/* CLIENTE -> SERVIDOR -> PROGRAMA */

t_package *get_package(int socket, t_log *logger);

t_list *get_instrucciones(t_package *paquete);
t_pcontexto *get_pcontexto(t_package *paquete);
t_pcontexto_desalojo *get_pcontexto_desalojo(t_package *paquete);
t_open_files *get_ofile(t_package *paquete);
t_data* get_data(t_package* paquete);
t_address get_address(t_package* paquete);
t_instruccion *get_instruccion(t_package *paquete);

t_list *get_ltsegmentos(t_package *paquete);
t_segments_table *get_tsegmento(t_package *paquete);
t_segment *get_segment(t_package *paquete);
t_status_code get_status_code(t_package *paquete);
t_pid_instruccion *get_pid_instruccion(t_package *paquete);

t_pid_status *get_pid_status(t_package *paquete);

t_info_write *get_info_write(t_package* paquete);
t_info_read *get_info_read(t_package* paquete);
t_info *get_info(t_package* paquete);

/* PROGRAMA -> CLIENTE -> SERVIDOR */

bool send_instrucciones(int socket, t_list *lista_instrucciones, t_log *logger);
bool send_pcontexto(int socket, t_pcontexto *contexto, t_log *logger);
bool send_pcontexto_desalojo(int socket, t_pcontexto_desalojo *contexto, t_log *logger);
bool send_ofile(int socket, t_open_files *t_ofiles, t_log *logger);
bool send_data(int socket, t_data* data, t_log *logger);
bool send_address(int socket,t_address address, t_log* logger);
bool send_instruccion(int socket, t_instruccion *instruccion, t_log *logger);
bool send_end(int socket, t_log *logger);
bool send_exit(int socket, t_log *logger);

bool send_ltsegmentos(int socket, t_list *lt_segmentos, t_log *logger);
bool send_tsegmento(int socket, t_segments_table *t_segmento, t_log *logger);
bool send_segment(int socket, t_segment *segmento, t_log *logger);
bool send_status_code(int socket, t_status_code status_code, t_log *logger);
bool send_pid_instruccion(int socket, t_pid_instruccion *pid_instruccion, t_log *logger);
bool send_compactar(int socket, t_log *logger);

bool send_pid_status(int socket, t_pid_status *pid_status, t_log *logger);

bool send_info_write(int socket, t_info_write *info_write, t_log *logger);
bool send_info_read(int socket, t_info_read *info_read, t_log *logger);
bool send_info(int socket, t_info *info, t_log *logger);

/* HANDSHAKE */

typedef enum
{
    HSCONSOLA,
    HSKERNEL,
    HSCPU,
    HSFS,
    HSMEMORIA,
    HSOK,
    HSFAIL
} hs_code;

/**
    * @NAME: hs_server_to_module_valid
    * @DESC: Funcion para recibir un handshake desde un cliente y validarlo
             En este caso no se controla en el servidor que al modulo que espero
             Al no contolarse, el cliente actua como Maestro y el servidor como Esclavo
             Se debe permitir al cliente que envie cualquier tipo de operacion
             Se procede con la primicia de que el cliente sabe lo que tiene que enviar
    * @PARAMS:
    *        socket_server - el socket del cliente
    *        hs_module_origin - el modulo que envia el handshake
    *        logger - el logger para escribir los mensajes
    * @RETURN:
    *        retorna true en caso de que el handshake sea valido, false en caso contrario
    */
bool hs_server_to_module_valid(int socket_client, hs_code hs_module_origin, t_log *logger);

int hs_server_to_module_get_type(int socket_client, hs_code hs_module_origin, t_log *logger);

/**
 * @NAME: hs_client_to_module_valid
 * @DESC: Funcion para enviar un handshake a un servidor y validar la conexion esperada a un modulo
 * @PARAMS:
 *        socket_server - el socket del servidor
 *        hs_module_origin - el modulo que envia el handshake
 *        hs_module_expected - el modulo que espera recibir
 *        logger - el logger para escribir los mensajes
 * @RETURN:
 *        retorna true en caso de que el handshake sea valido, false en caso contrario
 */
bool hs_client_to_module_valid(int socket_server, hs_code hs_module_origin, hs_code hs_module_expected, t_log *logger);

/* Cliente - Servidor */

void socket_destroy(int);

#endif /* COMMUNICATION_H_ */
