#include "kernel_vars.h"

t_log *logger_main = NULL;
t_log *logger_aux = NULL;

t_config_kernel *config_kernel = NULL;
t_modules_client *modules_client = NULL;
t_queues *queues = NULL;

int server_socket = -1;
bool accept_connections = true;

uint32_t pid_counter = 0;

pthread_t thr_server;
pthread_t thr_core;

bool core_running = true;