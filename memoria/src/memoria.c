#include "memoria.h"

int main(void) {
	t_log* logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);

	t_config* config;

	char* port;
	char* memory_size;
	char* segment_zero_size;
	char* segment_quantity;
	char* memory_time_delay;
	char* compactation_time_delay;
	char* compactation_algorithm;
	
	config = start_config();

	log_info(logger,"Configuracion cargada");

	port = config_get_string_value(config,"PUERTO_ESCUCHA");
	memory_size = config_get_string_value(config,"TAM_MEMORIA");
	segment_zero_size = config_get_string_value(config,"TAM_SEGMENTO_0");
	segment_quantity = config_get_string_value(config,"CANT_SEGMENTOS");
	memory_time_delay = config_get_string_value(config,"RETARDO_MEMORIA");
	compactation_time_delay = config_get_string_value(config,"RETARDO_COMPACTACION");
	compactation_algorithm = config_get_string_value(config,"ALGORITMO_ASIGNACION");

	int server_fd = start_server(port);
	log_info(logger, "Servidor listo para recibir al cliente");
	//Descomentar cuando se conecten los modulos que deberian
	//int client_fd = wait_client(server_fd);
	//Borrar linea siguiente tambien
	int client_fd = 1;

	printf("Puerto: %s \n",port);
	printf("Tamanio Memoria: %s \n",memory_size);
	printf("Tamanio Segmento 0: %s \n",segment_zero_size);
	printf("Cantidad de Segmentos: %s \n",segment_quantity);
	printf("Retraso Memoria: %s \n",memory_time_delay);
	printf("Retraso de Compactacion: %s \n",compactation_time_delay);
	


	/* Esto lo deje aca por si sirve

	t_list* lista;
	while (1) {
		int cod_op = get_operation(cliente_fd);
		switch (cod_op) {
		case 0:
			get_message(cliente_fd);
			break;
		case 1:
			lista = get_package(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida");
			break;
		}
	}

	*/

	finish_program(logger,config,server_fd, client_fd);

	return EXIT_SUCCESS;
};

void iterator(char* value) {
	log_info(logger,"%s", value);
};

t_config* start_config(void)
{
	t_config* new_config;

	new_config = config_create("./memoria.config");

	if(new_config==NULL)
	  {printf("No se encontro el archivo de configuracion");
	   exit(1);
	  }
	else
	  {return new_config;}
};

void finish_program(t_log* logger,t_config* config, int socket_server, int socket_client){
	if(logger != NULL)
	  {log_destroy(logger);}
	if(config != NULL)
	  {config_destroy(config);}
	if(socket_server != NULL)
	  {server_destroy(socket_server);}
	if(socket_client != NULL)
	  {client_destroy(socket_client);}
};