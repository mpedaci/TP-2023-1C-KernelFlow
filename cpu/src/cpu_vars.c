#include "cpu_vars.h"

int socket_client_memoria = -1;

bool ejecutando = false;
bool seg_fault = false;

t_log* logger = NULL;
t_log* logger_aux = NULL;

t_config_cpu* config = NULL;

t_registers* cpu_registers = NULL;