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
      t_arguments* arguments = malloc(sizeof(t_arguments));
      arguments->socket_client = client_wait(server_fd,logger);
      arguments->log = logger;
      pthread_t conection;
      pthread_create(&conection,0,handle_client,(void*)arguments);
      pthread_detach(conection);
      free(arguments);
   }
}

void* handle_client(void* arguments){
   int handshake;
   t_arguments* arg = (t_arguments*) arguments;
   int client_fd = arg->socket_client;
   recv(client_fd,&handshake,sizeof(int),MSG_WAITALL);
   //Deberia mandarle un send diciendo que se conecto correctamente?
   switch(handshake){
      case HSKERNEL:
		   log_info(arg->log,"Se conecto el modulo Kernel");
		   kernel_operations(client_fd, arg->log);
         break;
	   case HSCPU:
		   log_info(arg->log,"Se conecto el modulo CPU");
		   cpu_operations(client_fd, arg->log);
         break;
	   case HSFS:
		   log_info(arg->log,"Se conecto el modulo File System");
		   fs_operations(client_fd,arg->log);
         break;
	   default:
		   log_warning(arg->log,"Cliente desconocido");
		}
   socket_destroy(arg->socket_client);
   free(arg);
   return NULL;
}

