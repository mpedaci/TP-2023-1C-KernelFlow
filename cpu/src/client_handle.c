#include "client_handle.h"

void start_memory_client(char *ip, char *port, t_log *logger)
{
    int client_socket = create_connection(ip, port, logger);
    if (client_socket == -1)
    {
        log_error(logger, "No se pudo conectar al servidor de memoria");
        return;
    }
    if (!hs_client_to_module_valid(client_socket, HSCPU, HSMEMORIA, logger)) {
        log_error(logger, "Conexion no paso el handshake");
        return;
    }
        
    log_info(logger, "Conectado al servidor de memoria");
    // send_message("Hola, soy un cliente", client_socket);
    // log_info(logger, "Mensaje enviado");
    socket_destroy(client_socket);
}