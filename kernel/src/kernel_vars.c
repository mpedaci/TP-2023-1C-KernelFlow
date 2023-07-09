#include "kernel_vars.h"

t_log *logger_main = NULL;
t_log *logger_aux = NULL;

t_config_kernel *config_kernel = NULL;
t_modules_client *modules_client = NULL;

int server_socket = -1;
bool accept_connections = true;

bool end_program_flag = false;

t_queues *queues = NULL;

uint32_t pid_counter = 0;

pthread_t thr_server;
pthread_t thr_server_conn;
pthread_t thr_core;

pthread_mutex_t mutex_NEW_to_READY;

bool core_running = true;
bool compactation_flag = false;
bool fs_blocked = false;

t_list *all_pcb = NULL; 

// RECURSOS
t_list *recursos = NULL;

// Archivos
t_list *archivos_abiertos = NULL;

// IO
pthread_t thr_io;

// Archivos en FS
pthread_t thr_file;
pthread_mutex_t mutex_fs_connection;