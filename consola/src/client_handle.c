#include "client_handle.h"

int start_console_client(char* ip_kernel, char* puerto_kernel, t_log* logger_console){
  
    int kernel_socket = create_connection(ip_kernel, puerto_kernel);

    if(kernel_socket == -1){
        log_error(logger_console, "No se pudo conectar al servidor Kernel");
        return -1;
    }
    
    log_debug(logger_console, "Conexion exitosa con kernel: %i", kernel_socket);

    return kernel_socket;
}

