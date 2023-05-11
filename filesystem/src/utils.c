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

   // agrego los paths
   filesystem_config->path_superbloque = string_duplicate(config_get_string_value(config, "PATH_SUPERBLOQUE"));
   filesystem_config->path_bitmap = string_duplicate(config_get_string_value(config, "PATH_BITMAP"));
   filesystem_config->path_bloques = string_duplicate(config_get_string_value(config, "PATH_BLOQUES"));
   filesystem_config->path_fcb = string_duplicate(config_get_string_value(config, "PATH_FCB"));

   filesystem_config->retardo_acceso_bloque = string_duplicate(config_get_string_value(config, "RETARDO_ACCESO_BLOQUE"));

   log_info(logger, "Archivo de configuración leído correctamente");

   config_destroy(config);
   return filesystem_config;
}

void start_filesystem_server(char *listen_port, t_log *logger)
{
   int server_socket = server_start(listen_port, logger);
   log_info(logger, "servidor listo para escuchar a mi cliente");
   int client_socket = client_wait(server_socket, logger);
   log_info(logger, "se conecto el cliente correctamente");

   // Cierro todos sockets
   socket_destroy(client_socket);
   socket_destroy(server_socket);
}

void start_memory_client(char *ip, char *port, t_log *logger)
{
   int memory_connection = create_connection(ip, port, logger);

   close(memory_connection); // la cierro pa q no queda abierta al pedo
   socket_destroy(memory_connection);
}

void end_program(t_log *logger_main,t_config_filesystem *config,t_log *logger_aux)
{
   log_info(logger_aux, "Finalizando programa");
   // Logs destroy
   log_destroy(logger_main);
   log_destroy(logger_aux);
   // filesystem Config destroy
   free(config->ip_memoria);
   free(config->puerto_escucha);
   free(config->puerto_memoria);
   free(config->path_bitmap);
   free(config->path_bloques);
   free(config->path_fcb);
   free(config->path_superbloque);
   free(config->retardo_acceso_bloque);
   free(config);
}
