#ifndef KERNEL_STRUCTS_H
#define KERNEL_STRUCTS_H

#include <stdint.h>

#include <commons/collections/list.h>

typedef enum
{
    QNEW,
    QREADY,
    QEXEC,
    QBLOCK,
    QEXIT
} t_queue_id;

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

typedef struct
{
    char *recurso;
    int instancias;
    t_list *lista_bloqueados;
    pthread_mutex_t mutex;
} t_recurso;

typedef struct
{
    t_recurso *archivo;
    int puntero;
} t_archivo_abierto;

typedef struct
{
    int t_sleep;
    int pid;
} t_io_pcb;

typedef struct 
{
    int pid;
    t_status_code status_expected;
} t_pcb_file_status;


#endif /* KERNEL_STRUCTS_H */