#ifndef KERNEL_VARS_H
#define KERNEL_VARS_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include <commons/log.h>

#include "kernel_structs.h"

extern t_log *logger_main;
extern t_log *logger_aux;

extern t_config_kernel *config_kernel;
extern t_modules_client *modules_client;

extern int server_socket;
extern bool accept_connections;

extern t_queues *queues;

extern uint32_t pid_counter;

extern pthread_t thr_server;
extern pthread_t thr_core;

extern bool core_running;

#endif /* KERNEL_VARS_H */