#include "consola.h"



int main(int argc, char** argv){ 

   char* ip_kernel;
   char* puerto_kernel;

   t_log* logger_console;
   t_config* config_console;

   int kernel_socket;

   FILE* archivo_instrucciones = fopen(argv[1], "r");

   logger_console = log_create("consola.log", "Consola", true, LOG_LEVEL_DEBUG);

   if(logger_console == NULL){
      log_error(logger_console, "No se pudo iniciar la consola"),
      destroy_console(config_console, logger_console, kernel_socket);
   }

   log_info(logger_console, "Consola iniciada.");

   config_console = initialize_config_console("/home/utnso/Documents/tp-2023-1c-KernelFlow/consola/bin/console.config");

   ip_kernel = config_get_string_value(config_console, "IP_KERNEL");
   puerto_kernel = config_get_string_value(config_console, "PUERTO_KERNEL");

   kernel_socket = create_connection(ip_kernel, puerto_kernel); 

   /*-------PARSEAR PSEUDOCODIGO--------

   if(!el_proceso_es_valido(archivo_instrucciones)){
      log_error(logger_console, "El proceso no es valido");
      destroy_console(config_console, logger_console, kernel_socket);
   }

   ------ENVIAR LISTA INSTRUCCIONES A KERNEL--------

   send_package(archivo_instrucciones, kernel_socket);

   wait_kernel(kernel_socket);*/

   destroy_console(config_console, logger_console, kernel_socket);
   
   return 0;
};

t_config* initialize_config_console(char* config_path){

    t_config* config_console = config_create(config_path);
    
    if(config_console == NULL){
      printf("NO se pudo configurar el archivo de configuraciones");
      exit(1);
    }

    else{
      return config_console; }
};

void destroy_console(t_config* config_console, t_log* logger_console, int kernel_socket){
    if(logger_console != NULL){
      log_destroy(logger_console);
    }
    if(config_console != NULL){
      config_destroy(config_console);
    }
    close(kernel_socket);
   }

wait_kernel(int kernel_socket){}
