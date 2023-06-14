#ifndef MODULOS_H
#define MODULOS_H

#include "utils.h"
#include "estructuras.h"

void kernel_operations(int client_socket);
void cpu_operations(int client_socket);
void fs_operations(int client_socket);
//void cpu_and_fs_operations(int client_socket,char *module);

#endif /* MODULOS_H */