#include "filesystem_vars.h"

t_log *logger_main = NULL;
t_log *logger_aux = NULL;

t_config_filesystem *config_fs = NULL;

int memory_socket = -1;

t_superbloque *superbloque = NULL;
t_bitarray *bitmap = NULL;

void *blocks = NULL;

int bitmap_file = -1;
int blocks_file = -1;

pthread_t thr_requestAsync;