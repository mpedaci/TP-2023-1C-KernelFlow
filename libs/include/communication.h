#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h> // send - recv
#include <unistd.h> // close

#include <commons/log.h>

#include "types.h"
#include "package.h"

/* OPERACIONES DE ENVIO DE INFORMACION */

typedef enum
{
    INSTRUCCIONES, // Pseudocodigo
    PCONTEXTO,     // Contexto de proceso
    TSEGMENTOS,    // Tabla de Segmentos
    OFILE,         // Archivo abierto
    DFILE,         // Datos Archivo
    DATA,          // Datos
    INSTRUCCION,   // Instruccion
    END
} op_code;


/* CLIENTE -> SERVIDOR -> PROGRAMA */

t_package* get_package(int socket, t_log *logger);

t_persona* get_instrucciones(t_package* paquete);
t_pcontexto* get_pcontexto(t_package* paquete);
t_persona* get_tsegmento(t_package* paquete);
t_persona* get_ofile(t_package* paquete);
t_persona* get_file(t_package* paquete);
t_persona* get_data(t_package* paquete);
t_persona* get_instruccion(t_package* paquete);

/* PROGRAMA -> CLIENTE -> SERVIDOR */

bool send_instrucciones(int socket, t_persona persona, t_log *logger);
bool send_pcontexto(int socket, t_pcontexto* contexto, t_log *logger);
bool send_tsegmento(int socket, t_persona persona, t_log *logger);
bool send_ofile(int socket, t_persona persona, t_log *logger);
bool send_file(int socket, t_persona persona, t_log *logger);
bool send_data(int socket, t_persona persona, t_log *logger);
bool send_instruccion(int socket, t_persona persona, t_log *logger);
bool send_end(int socket, t_log *logger);

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
