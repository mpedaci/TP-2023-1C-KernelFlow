#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include "server_handle.h"

// Hardcodeado
char* logger_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/cpu/logs/cpu.log";
char* logger_aux_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/cpu/logs/cpu_aux.log";
char* config_path = "/home/utnso/Documents/tp-2023-1c-KernelFlow/cpu/config/cpu.config";


void sigintHandler(int signum);

void print_config(t_config_cpu *config);

void print_contexto(t_pcontexto *contexto);

void print_contexto_desalojo(t_pcontexto_desalojo *contexto);

void iterator_print_instruction(t_instruccion *instruccion);

void print_registers(t_registers *registers);

#endif