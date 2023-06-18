#ifndef MEMORIA_H
#define MEMORIA_H

#include <signal.h> // Para cierre seguro

#include "print_information.h"
#include "estructuras.h"
#include "memory_server.h"

char *config_path = "./config/memoria.config";
char *logger_main_path = "./logs/memoria.log";
char *logger_aux_path = "./logs/memoria_aux.log";

int main();
void sigintHandler(int signum); // Para cierre seguro

#endif /* MEMORIA_H */