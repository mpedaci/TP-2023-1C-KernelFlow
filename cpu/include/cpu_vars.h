#ifndef CPU_VARS_H
#define CPU_VARS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"


extern bool ejecutando;

extern t_log* logger;
extern t_log* logger_aux;

extern t_config_cpu* config;

extern t_registers* cpu_registers;

#endif