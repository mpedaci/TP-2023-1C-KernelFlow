#ifndef MODULOS_H
#define MODULOS_H

#include "compactacion.h"

void kernel_operations(int client_socket);
void handle_pid_instruction(int client_socket, t_pid_instruccion *pidtruction);
void cpu_operations(int client_socket);
void fs_operations(int client_socket);

#endif /* MODULOS_H */