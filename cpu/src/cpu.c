#include "cpu.h"



int main() {

   char* instruction_delay;
   char* ip;
	char* memory_port;
   char* listening_port;
   char* max_segment_size;


   // creo logger
   logger = log_create("../log.log", "CPU", 1, LOG_LEVEL_DEBUG);

   if(logger == NULL) {
      close_program(memory_port, listening_port, config, logger);
   }

   // creo y traigo las config
   config = create_config("../cpu.config");

   if(config == NULL) {
      close_program(memory_port, listening_port, config, logger);
   }

   instruction_delay = config_get_string_value(config, "RETARDO_INSTRUCCION");
   ip = config_get_string_value(config, "IP");
	memory_port = config_get_string_value(config,"PUERTO_MEMORIA");
   listening_port = config_get_string_value(config,"PUERTO_ESCUCHA");
   max_segment_size = config_get_string_value(config, "TAM_MAX_SEGMENTO");


   // creo server para kernel
   int socket_serverCPU = start_server(listening_port);
   log_info(logger, "Servidor listo para recibir cliente!");
   int socket_clientKernel = wait_client(socket_serverCPU);

   //recibir mensajes del cliente HACER


   // creo conexion a memoria
   int memory_conexion = create_conexion(memory_port);
   // char* handshake = "hola"; //preguntar como vamos a hacer el handshake (despues hacer con malloc)
   // send_message(handshake, memory_conexion);
   // HABLAR TEMA DE HANDSHAKE CON EL GRUPETE




   





   close_program(memory_port, listening_port, config, logger);

   return 0;
}

void close_program(int memory_port, int listening_port, t_config* config, t_log* logger) {
   client_destroy(memory_port);
   server_destroy(listening_port);
   config_destroy(config);
   log_destroy(logger);
}
