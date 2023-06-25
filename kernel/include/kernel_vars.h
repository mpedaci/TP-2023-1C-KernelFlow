#ifndef KERNEL_VARS_H
#define KERNEL_VARS_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include <commons/log.h>

#include "kernel_structs.h"

extern t_log *logger_main;
extern t_log *logger_aux;

extern t_config_kernel *config_kernel;
extern t_modules_client *modules_client;

extern int server_socket;
extern bool accept_connections;

extern bool end_program_flag;

extern t_queues *queues;

extern uint32_t pid_counter;

extern pthread_t thr_server;
extern pthread_t thr_server_conn;
extern pthread_t thr_core;

extern bool core_running;

extern t_list *all_pcb;

// RECURSOS
extern t_list *recursos;

// Archivos
extern t_list *archivos_abiertos;

// IO
extern pthread_t thr_io;

// Archivos
extern pthread_t thr_file;

#endif /* KERNEL_VARS_H */