#ifndef KERNEL_STRUCTS_H
#define KERNEL_STRUCTS_H

#include <stdint.h>

#include <commons/collections/list.h>

typedef struct
{
    // Configuración del cliente
    char *ip_memoria;                // Dirección IP de la memoria
    char *puerto_memoria;            // Puerto de la memoria
    char *ip_filesystem;             // Dirección IP del filesystem
    char *puerto_filesystem;         // Puerto del filesystem
    char *ip_cpu;                    // Dirección IP de la CPU
    char *puerto_cpu;                // Puerto de la CPU
    // Configuración del servidor
    char *puerto_escucha;            // Puerto escucha conexiones entrantes
    // Algoritmo Planificacion
    char *algoritmo_planificacion;
    double estimacion_inicial;
    double hrrn_alfa;
    // Multiprogramacion
    int grado_max_multiprog;
    // Perifericos
    t_list *recursos;
    t_list *instancias_recursos;
} t_config_kernel;

typedef struct
{
    int memory_client_socket;
    int filesystem_client_socket;
    int cpu_client_socket;
} t_modules_client;

typedef struct
{
    t_list *NEW;
    t_list *READY;
    t_list *EXEC;
    t_list *BLOCK;
    t_list *EXIT;
} t_queues;

typedef struct
{
    int socket;
    uint32_t pid;
} t_client_connection;

#endif /* KERNEL_STRUCTS_H */