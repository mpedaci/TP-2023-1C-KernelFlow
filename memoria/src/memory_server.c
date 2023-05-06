#include "memory_server.h"

void start_memory_server(char* port, t_log * logger)
{  log_info(logger, "Iniciando servidor");
	int server_fd = server_start(port, logger);
	log_info(logger, "Servidor listo para recibir al cliente");
	//Descomentar cuando se conecten los modulos que deberian
   wait_clients(server_fd,logger);
   socket_destroy(server_fd);
}

void wait_clients(int server_fd,t_log* logger){
   while(server_fd != -1){ //No estoy muy seguro de esta condicion
      int client_fd = client_wait(server_fd,logger);
      pthread_t* conection;
      pthread_create(&conection,0,handle_client,((void*)client_fd,(void*) logger));
      pthread_detach(conection);
      socket_destroy(client_fd);
   }
}

void handle_client(int client_fd,t_log* logger){
   int handshake;
   recv(client_fd,&handshake,sizeof(int),MSG_WAITALL);
   //Deberia mandarle un send diciendo que se conecto correctamente?
   switch(handshake){
      case HSKERNEL:
		   log_info(logger,"Se conecto el modulo Kernel");
		   kernel_operations(client_fd, logger);
         break;
	   case HSCPU:
		   log_info(logger,"Se conecto el modulo CPU");
		   cpu_operations(client_fd, logger);
         break;
	   case HSFS:
		   log_info(logger,"Se conecto el modulo File System");
		   fs_operations(client_fd,logger);
         break;
	   default:
		   log_warning(logger,"Cliente desconocido");
         return;
		}
   free(handshake);
}

