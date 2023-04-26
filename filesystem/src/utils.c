#include <utils.h>

t_config_filesystem *read_config(char *config_path, t_log *logger)
{
   t_config_filesystem *filesystem_config = malloc(sizeof(t_config_filesystem));
   t_config *config = config_create(config_path);

   if (config == NULL)
   {
      log_error(logger, "No se pudo leer el archivo de configuración");
      return NULL;
   }

   filesystem_config->ip_memoria = string_duplicate(config_get_string_value(config, "IP_MEMORIA"));
   filesystem_config->puerto_memoria = string_duplicate(config_get_string_value(config, "PUERTO_MEMORIA"));
   filesystem_config->puerto_escucha = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA"));

   //agrego los paths 
   filesystem_config->path_superbloque = string_duplicate(config_get_string_value(config, "PATH_SUPERBLOQUE"));
   filesystem_config->path_bitmap = string_duplicate(config_get_string_value(config, "PATH_BITMAP"));
   filesystem_config->path_bloques = string_duplicate(config_get_string_value(config, "PATH_BLOQUES"));
   filesystem_config->path_fcb = string_duplicate(config_get_string_value(config, "PATH_FCB"));


   log_info(logger, "Archivo de configuración leído correctamente");

   config_destroy(config);
   return filesystem_config;
}


void start_filesystem_server(char *listen_port, t_log *logger)
{
    int server_socket = start_server(listen_port);
    log_info(logger, "servidor listo para escuchar a mi cliente");
    int client_socket = wait_client(server_socket);
    log_info(logger, "se conecto el cliente correctamente");

    // Cierro todos sockets
    client_destroy(client_socket);
    server_destroy(server_socket);
}


void start_memory_client(char *ip, char *port, t_log *logger)
{
    int memory_connection= create_connection(ip,port);

    close(memory_connection); //la cierro pa q no queda abierta al pedo
    destroy_connection(memory_connection);

}


void end_program(t_log *logger_main, t_log *logger_aux, t_config_filesystem *config)
{
   log_info(logger_aux, "Finalizando programa");
   // Logs destroy
   log_destroy(logger_main);
   log_destroy(logger_aux);
   // filesystem Config destroy
   free(config->ip_memoria); 
   free(config->puerto_escucha);
   free(config->puerto_memoria); 
   free(config);
}