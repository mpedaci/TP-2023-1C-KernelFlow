#include "kernel_vars.h"

t_log *logger_main = NULL;
t_log *logger_aux = NULL;

t_config_kernel *config_kernel = NULL;
t_modules_client *modules_client = NULL;
t_queues *queues = NULL;

int server_socket = -1;
bool accept_connections = true;

bool end_program_flag = false;

uint32_t pid_counter = 0;

pthread_t thr_server;
pthread_t thr_server_conn;
pthread_t thr_core;

bool core_running = true;

t_list *all_pcb = NULL; 

t_list *recursos = NULL;

t_list *archivos_abiertos = NULL;

pthread_t thr_io;

pthread_t thr_file;

pthread_mutex_t mutex_new;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_running;
pthread_mutex_t mutex_blocked;
pthread_mutex_t mutex_exit;
pthread_mutex_t mutex_pid;