#include "memory_server.h"

void start_memory_server(char *port)
{
   log_info(logger_aux, "Iniciando servidor");
   int server_fd = server_start(port, logger_aux);
   log_info(logger_aux, "Servidor listo para recibir al cliente");
   wait_clients(server_fd);
   socket_destroy(server_fd);
}

void wait_clients(int server_fd)
{
   while (server_fd != -1)
   { // No estoy muy seguro de esta condicion
      char *socket_client = string_itoa(client_wait(server_fd, logger_aux));
      pthread_t conection;
      pthread_create(&conection, 0, handle_client, (void *)socket_client);
      pthread_detach(conection);
   }
}

void *handle_client(void *socket_client)
{
   int socket = atoi((char *)socket_client);
   int HS_module = hs_server_to_module_get_type(socket, HSMEMORIA, logger_aux);

   switch (HS_module)
   {
   case HSKERNEL:
      log_info(logger_aux, "Se conecto el modulo Kernel");
      kernel_operations(socket);
      break;
   case HSCPU:
      log_info(logger_aux, "Se conecto el modulo CPU");
      cpu_operations(socket);
      break;
   case HSFS:
      log_info(logger_aux, "Se conecto el modulo File System");
      fs_operations(socket);
      break;
   default:
      log_warning(logger_aux, "Cliente desconocido");
      break;
   }
   socket_destroy(socket);
   pthread_exit(0);
}