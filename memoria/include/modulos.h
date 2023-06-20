#ifndef MODULOS_H
#define MODULOS_H

#include "compactacion.h"
#include "types_controllers.h"

void kernel_operations(int client_socket);
void handle_kernel_pid_instruction(int client_socket, t_pid_instruccion *pidtruction);
void handle_pid_status(int client_socket, t_pid_status *pid_status);
void cpu_operations(int client_socket);
void handle_cpu_pid_instruction(int client_socket, t_pid_instruccion *pidtruction);
void fs_operations(int client_socket);
//void cpu_and_fs_operations(int client_socket,char *module);

void instruction_destroyer(t_instruccion *instruccion);

#endif /* MODULOS_H */