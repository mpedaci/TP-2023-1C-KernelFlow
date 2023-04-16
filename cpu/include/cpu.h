#ifndef CPU_H
#define CPU_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include "server.h"
#include "client.h"
#include "utils.h"
#include <stdbool.h>
#include <pthread.h>
#include "a.h"

char* logger_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/cpu/log/cpu.log";
char* config_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/cpu/conf/cpu.config";

// Instalar
// duplicate line ctrl+d


void close_program_cpu(int, int, int, t_config*, t_log*);
void iterator(char*);

#endif