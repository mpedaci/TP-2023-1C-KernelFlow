#include "client_handle.h"

int start_memory_client(char *ip, char *port, t_log *logger)
{
    int client_socket = create_connection(ip, port, logger);

    if (client_socket == -1)
    {
        log_error(logger, "No se pudo conectar al servidor de memoria");
        return -1;
    }
    if (!hs_client_to_module_valid(client_socket, HSCPU, HSMEMORIA, logger)) {
        log_error(logger, "Conexion no paso el handshake");
        return -1;
    }
        
    log_info(logger, "Conectado al servidor de memoria");
    return client_socket;
}