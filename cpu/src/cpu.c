#include "cpu.h"



int main() {
   // variables para el config
   char* instruction_delay;
   char* ip;
	char* memory_port;
   char* listening_port;
   char* max_segment_size;

   // logger y config
   t_log* logger;
   t_config* config;

   // variables para los sockets
   int memory_conexion;
   int socket_serverCPU; 
   int socket_clientKernel;


   // creo logger
   logger = log_create("../log.log", "CPU", 1, LOG_LEVEL_DEBUG);

   if(logger == NULL) {
      close_program_cpu(memory_conexion, socket_clientKernel, socket_serverCPU, config, logger);
   }

   // creo y traigo las config
   config = create_config("../cpu.config");

   if(config == NULL) {
      close_program_cpu(memory_conexion, socket_clientKernel, socket_serverCPU, config, logger);
   }

   instruction_delay = config_get_string_value(config, "RETARDO_INSTRUCCION");
   ip = config_get_string_value(config, "IP");
	memory_port = config_get_string_value(config,"PUERTO_MEMORIA");
   listening_port = config_get_string_value(config,"PUERTO_ESCUCHA");
   max_segment_size = config_get_string_value(config, "TAM_MAX_SEGMENTO");


   // creo server para kernel
   socket_serverCPU = start_server(listening_port);
   log_info(logger, "Servidor listo para recibir cliente!");
   socket_clientKernel = wait_client(socket_serverCPU);

   t_list* list;
	while (1) {
		int cod_op = get_operation(socket_clientKernel);
		switch (cod_op) {
		case MENSAJE:
			get_message(socket_clientKernel);
			break;
		case PAQUETE:
			list = get_package(socket_clientKernel);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(list, (void*) iterator);
			break;
		case -1:
			log_error(logger, "El cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}


   // creo conexion a memoria
   memory_conexion = create_conexion(memory_port);
   // char* handshake = "hola";
   // send_message(handshake, memory_conexion);
   // HABLAR TEMA DE HANDSHAKE CON EL GRUPETE ##############################

   

   // HACER ERROR HANDLING EN LAS CONEXIONES #################################


   





   close_program_cpu(memory_conexion, socket_clientKernel, socket_serverCPU, config, logger);

   return 0;
}

void close_program_cpu(int memory_conexion, int socket_clientKernel, int socket_serverCPU, t_config* config, t_log* logger) {
   destroy_connection(memory_conexion);
   client_destroy(socket_clientKernel);
   server_destroy(socket_serverCPU);
   config_destroy(config);
   log_destroy(logger);
}

void iterator(char* value) {
   // que hacer con cada mensaje que nos manda el cliente en el paquete
   log_info(logger,"%s", value);
}