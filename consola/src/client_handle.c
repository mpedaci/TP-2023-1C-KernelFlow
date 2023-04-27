#include "client_handle.h"

int create_connection2(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = 0;
	socket_cliente = socket(server_info->ai_family,
							server_info->ai_socktype,
							server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}

int start_console_client(char* ip_kernel, char* puerto_kernel, t_log* logger_console){
  
    int kernel_socket = create_connection2(ip_kernel, puerto_kernel);

    if(kernel_socket == -1){
        log_error(logger_console, "No se pudo conectar al servidor Kernel");
        return -1;
    }
    /*if (!hs_client_to_module_valid(kernel_socket, HSCONSOLA, HSKERNEL, logger_console))
        return -1;*/

    log_debug(logger_console, "Conexion exitosa con kernel");

    return kernel_socket;
}


