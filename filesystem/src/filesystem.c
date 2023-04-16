#include "filesystem.h"
int main() {

t_log* logger; 
t_config* config; 
int conexion_memoria; 
char* puerto_escucha; 
char* puerto_memoria;
char* ip_memoria;


int server_socket; //var nueva pa el socket
int client_socket;  // var nueva pa el socket 


//configuraciones iniciales 

logger= log_create("filesystem.log", "filesystem", true, LOG_LEVEL_INFO);
log_info(logger, "log creado");

config= config_create("../config/filesystem.config");

if(config == NULL){
    log_info(config, "Eror al crear la config");
    end_program(config,logger,conexion_memoria,server_socket,client_socket);

}

log_info(logger, "config creado");


//filesystem en modo cliente de memoria

puerto_memoria= config_get_string_value(config,"PUERTO_MEMORIA");
ip_memoria= config_get_string_value(config,"IP_MEMORIA");
conexion_memoria= create_connection(ip_memoria,puerto_memoria);

close(conexion_memoria); // la cierro xq sino queda abierta al pedo ?

//filesystem en modo servidor de kernel

puerto_escucha= config_get_string_value(config,"PUERTO_ESCUCHA"); 
log_info(logger, "el puerto es: %s",puerto_escucha);
server_socket= start_server(puerto_escucha);
log_info(logger, "Listo para escuchar a mi cliente");
//log_info(logger, "filesystem corriendo en %s",puerto_escucha);
client_socket= wait_client(server_socket);
log_info(logger, "se conecto el cliente correctamente");

//terminar el programa y liberar todo

end_program(config,logger,conexion_memoria,server_socket,client_socket);
return EXIT_SUCCESS;

}

void end_program( t_config* config, t_log* logger, int conexion_memoria, int server_socket, int client_socket){
    log_destroy(logger);
    config_destroy(config);
    destroy_connection(conexion_memoria);
    server_destroy(server_socket);
    client_destroy(client_socket);
}