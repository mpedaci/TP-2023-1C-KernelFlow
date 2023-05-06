#ifndef MODULOS_H
#define MODULOS_H

#include "utils.h"
#include "estructuras.h"

void kernel_operations(int client_socket, t_log* logger);
void cpu_operations(int client_socket, t_log* logger);
void fs_operations(int client_socket, t_log* logger);

#endif /* MODULOS_H */