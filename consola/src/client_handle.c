#include "client_handle.h"

void start_console_client(char* ip_kernel, char* puerto_kernel, t_log* logger_console){
  
    int kernel_socket = create_connection(ip_kernel, puerto_kernel);

    if(kernel_socket == -1){
        log_error(logger_console, "No se pudo conectar al servidor Kernel");
        return;
    }
    
    /*if (!client_pass_handshake(client_socket, logger)){}
        return;
    log_info(logger, "Conectado al servidor de memoria");
    send_message("Hola, soy un cliente", client_socket);
    log_info(logger, "Mensaje enviado");*/

    destroy_connection(kernel_socket);
}
