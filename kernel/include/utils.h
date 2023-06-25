#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdint.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include "types.h"
#include "types_controllers.h"
#include "kernel_structs.h"

#include "communication.h"

// AUXILIAR FUNCTIONS

void add_to_list(char **source, t_list *destiny);
void free_arr(char **arr);

// KERNEL FUNCTIONS

t_config_kernel *read_config(char *path, t_log *logger);

t_pcb *pcb_create(uint32_t pid, t_list *instrucciones);

// Print

char *get_pids_queue_string(t_queue *q);

// END PROGRAM

void end_program(t_log *logger_main, t_log *logger_aux, t_config_kernel *config, t_modules_client *modules_client);

// FREE FUNCTIONS

void free_config_kernel(t_config_kernel *config);
void free_modules_client(t_modules_client *modules_client, t_log *logger);

#endif /* UTILS_H */