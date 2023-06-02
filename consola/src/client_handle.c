#include "client_handle.h"

int start_console_client(char* ip_kernel, char* puerto_kernel, t_log* logger_console){
    int kernel_socket = create_connection(ip_kernel, puerto_kernel, logger_console);
    if(kernel_socket == -1){
        log_error(logger_console, "No se pudo conectar al servidor Kernel");
        return -1;
    }
    if (!hs_client_to_module_valid(kernel_socket, HSCONSOLA, HSKERNEL, logger_console))
        return -1;
    log_debug(logger_console, "Conexion exitosa con kernel");
    return kernel_socket;
}
