#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct
{
    // Configuración del cliente
    char *ip_memoria;        // Dirección IP de la memoria
    char *puerto_memoria;    // Puerto de la memoria
    char *ip_filesystem;     // Dirección IP del filesystem
    char *puerto_filesystem; // Puerto del filesystem
    char *ip_cpu;            // Dirección IP de la CPU
    char *puerto_cpu;        // Puerto de la CPU
    // Configuración del servidor
    char *puerto_escucha; // Puerto en el que el kernel escucha las conexiones entrantes
} t_config_kernel;

t_config_kernel *read_config(char *path, t_log *logger);

void end_program(t_log *logger_main, t_log *logger_aux, t_config_kernel *config);


#endif /* UTILS_H */