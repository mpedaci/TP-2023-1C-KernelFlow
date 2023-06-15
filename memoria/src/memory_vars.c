#include "memory_vars.h"

t_log *logger_main = NULL;
t_log *logger_aux = NULL;

t_config_memoria *config = NULL;

bool accept_connections = false;
pthread_t connection;

t_segment *segment_0 = NULL;
t_list *all_segments_tables = NULL;

void *memory_space = NULL;

t_bitarray *free_space_table = NULL;