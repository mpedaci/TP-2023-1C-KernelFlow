#include "memory_vars.h"

t_log *logger_main = NULL;
t_log *logger_aux = NULL;

t_config_memoria *config = NULL;

bool end_program_flag = false;

int server_socket = -1;
bool accept_connections = true;

pthread_t thr_server;
pthread_t thr_server_conn;

bool fs_connected = false;
bool cpu_connected = false;
bool kernel_connected = false;

t_list *all_segments_tables = NULL;
t_segment *segment_0 = NULL;

void *memory_space = NULL;
pthread_mutex_t memory_space_mutex;

t_bitarray *free_space_table = NULL;