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
extern pthread_mutex_t mutex_NEW_to_READY;

extern bool core_running;
extern bool compactation_flag;
extern bool fs_blocked;

extern t_list *all_pcb;

// RECURSOS
extern t_list *recursos;

// Archivos
extern t_list *archivos_abiertos;

// IO
extern pthread_t thr_io;

// Archivos en FS
extern pthread_t thr_file;
extern pthread_mutex_t mutex_fs_connection;

#endif /* KERNEL_VARS_H */