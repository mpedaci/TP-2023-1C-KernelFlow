#include <utils.h>

t_config_memoria *read_config(char *config_path, t_log *logger)
{
   t_config_memoria *memoria_config = malloc(sizeof(t_config_memoria));
   t_config *config = config_create(config_path);

   if (config == NULL)
   {
      log_error(logger, "No se encontro el archivo de configuración");
      return NULL;
   }

   memoria_config->port = string_duplicate(config_get_string_value(config,"PUERTO_ESCUCHA"));
   memoria_config->memory_size = string_duplicate(config_get_string_value(config,"TAM_MEMORIA"));
   memoria_config->segment_zero_size = string_duplicate(config_get_string_value(config,"TAM_SEGMENTO_0"));
   memoria_config->segment_quantity = string_duplicate(config_get_string_value(config,"CANT_SEGMENTOS"));
   memoria_config->memory_time_delay = string_duplicate(config_get_string_value(config,"RETARDO_MEMORIA"));
   memoria_config->compactation_time_delay = string_duplicate(config_get_string_value(config,"RETARDO_COMPACTACION"));
   memoria_config->compactation_algorithm = string_duplicate(config_get_string_value(config,"ALGORITMO_ASIGNACION"));

   log_info(logger, "Archivo de configuración leído correctamente");

   config_destroy(config);
   return memoria_config;
}

void start_memory_server(char* port, t_log * logger)
{  log_info(logger, "Iniciando servidor");
	int server_fd = server_start(port, logger);
	log_info(logger, "Servidor listo para recibir al cliente");
	//Descomentar cuando se conecten los modulos que deberian
	//int client_fd = wait_client(server_fd);
	//Borrar linea siguiente tambien
	int client_fd = 1;

	/* 
	while (1) {
      int handshake = get_operation(cliente_fd);
		switch (handshake) {
		case HSKERNEL:
			log_info(logger_aux,"Se conecto el modulo Kernel");
			send_message("Conexión establecida Memoria - Kernel", client_fd)
		case HSCPU:
			log_info(logger_aux,"Se conecto el modulo CPU");
			send_message("Conexión establecida Memoria - CPU", client_fd)
		case HSFS:
			log_info(logger_aux,"Se conecto el modulo File System");
			send_message("Conexión establecida Memoria - File System", client_fd)
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Cliente desconocido");
			break;
		}
      free(handshake);
	}
	*/

   socket_destroy(client_fd);
   socket_destroy(server_fd);

}


void end_program(t_log *logger_main, t_log *logger_aux, t_config_memoria *config)
{
   log_debug(logger_aux, "Finalizando programa");
   // Logs destroy
   log_destroy(logger_main);
   log_destroy(logger_aux);
   // Memoria Config destroy
   free(config->port);
   free(config->memory_size);
   free(config->segment_zero_size);
   free(config->segment_quantity);  
   free(config->memory_time_delay);
   free(config->compactation_time_delay);
   free(config->compactation_algorithm); 
   free(config);
}
