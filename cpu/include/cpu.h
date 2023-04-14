#ifndef CPU_H
#define CPU_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include "server.h"
#include "client.h"
#include <stdbool.h>
#include <pthread.h>
#include "a.h"


void close_program_cpu(int, int, int, t_config*, t_log*);
void iterator(char*);

#endif