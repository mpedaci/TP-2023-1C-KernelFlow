#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <client.h> //agregado 
#include <server.h> //agregado

typedef struct
{
    char *puerto_escucha;        
    char *puerto_memoria;   
    char *ip_memoria;    

} t_config_filesystem;

t_config_filesystem *read_config(char *path, t_log *logger);
void end_program(t_log *logger_main, t_log *logger_aux, t_config_filesystem *config);
void start_memory_client(char *ip, char *port, t_log *logger);
void start_filesystem_server(char *listen_port, t_log *logger);



#endif /* UTILS_H */