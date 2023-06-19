#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdint.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include "types.h"
#include "kernel_structs.h"

#include "communication.h"

// AUXILIAR FUNCTIONS

void add_to_list(char **source, t_list *destiny);
void free_arr(char **arr);

// KERNEL FUNCTIONS

t_config_kernel *read_config(char *path, t_log *logger);

t_queues *create_queues();

t_pcb *pcb_create(uint32_t pid, t_list *instrucciones);

// END PROGRAM

void end_program(
    t_log *logger_main, t_log *logger_aux,
    t_config_kernel *config,
    t_modules_client *modules_client,
    t_list *all_pcb,
    t_queues *queues);

// FREE FUNCTIONS

t_registers *init_registers();

void free_config_kernel(t_config_kernel *config);
void free_modules_client(t_modules_client *modules_client, t_log *logger);
void free_registers(t_registers *registers);
void free_pcb(t_pcb *pcb);
void free_queues(t_queues *queues);
void free_instruccion(t_instruccion *instruccion);
void free_lista_instrucciones(t_list *lista_instrucciones);
void free_pcontexto_desalojo(t_pcontexto_desalojo *pcontexto);
void free_pcontexto(t_pcontexto *pcontexto);

void copy_registers(t_registers *dest, t_registers *src);
t_list *copy_instructions_list(t_list *instructions);
t_instruccion *new_instruction(t_instruccion *instruccion);

t_list *copy_segment_list(t_segments_table *segments_table);
t_segment *new_segment(t_segment *segment);

#endif /* UTILS_H */