#include "memory_server.h"

void start_memory_server(char *port)
{
   log_info(logger_aux, "Iniciando servidor");
   int server_fd = server_start(port, logger_aux);
   if (server_fd == -1)
   {
      log_error(logger_aux, "No se pudo iniciar el servidor");
   }
   else
   {
      log_info(logger_aux, "Servidor listo para recibir al cliente");
      accept_connections = true;
      wait_clients(server_fd);
   }
   socket_destroy(server_fd);
}

void wait_clients(int server_fd)
{
   while (accept_connections)
   { // No estoy muy seguro de esta condicion
      int socket_client = client_wait(server_fd, logger_aux);
      char *s_client = string_itoa(socket_client);
      if(socket_client != -1)
      {
         pthread_create(&connection, 0, handle_client, (void *)s_client);
      }
      else
      {
         free(s_client);
      }
   }
   pthread_join(connection, NULL);
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
   //free(socket_client);
   socket_destroy(socket);
   pthread_exit(0);
}