#include "memory_server.h"

void start_memory_server(char* port)
{  log_info(logger_aux, "Iniciando servidor");
	int server_fd = server_start(port, logger_aux);
	log_info(logger_aux, "Servidor listo para recibir al cliente");
   wait_clients(server_fd);
   socket_destroy(server_fd);
}

void wait_clients(int server_fd){
   while(server_fd != -1){ //No estoy muy seguro de esta condicion
      char* socket_client = malloc(sizeof(char*));
      sprintf(socket_client, "%d",client_wait(server_fd,logger_aux));
      pthread_t conection;
      pthread_create(&conection,0,handle_client,(void*)socket_client);
      pthread_detach(conection);
   }
}

void* handle_client(void* socket_client){
   int handshake;
   int client_fd = atoi((char*) socket_client);
   recv(client_fd,&handshake,sizeof(int),MSG_WAITALL);
   //Deberia mandarle un send diciendo que se conecto correctamente?
   switch(handshake){
      case HSKERNEL:
		   log_info(logger_aux,"Se conecto el modulo Kernel");
		   kernel_operations(client_fd);
         break;
	   case HSCPU:
		   log_info(logger_aux,"Se conecto el modulo CPU");
		   cpu_operations(client_fd);
         break;
	   case HSFS:
		   log_info(logger_aux,"Se conecto el modulo File System");
		   fs_operations(client_fd);
         break;
	   default:
		   log_warning(logger_aux,"Cliente desconocido");
		}
   socket_destroy(client_fd);
   return NULL;
}

