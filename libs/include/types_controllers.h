#ifndef TYPES_CONTROLLERS_H_
#define TYPES_CONTROLLERS_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <commons/temporal.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#include "types.h"

// CONSTRUCTORS

t_pid_status *pid_status_create(uint32_t pid, t_status_code status);
t_pcontexto *pcontexto_create(t_pcb *pcb);
t_registers *registers_create();

// DUPLICATORS

void registers_duplicate(t_registers *from, t_registers *to);
t_list *lista_instrucciones_duplicate(t_list *instructions);
t_instruccion *instruccion_duplicate(t_instruccion *instruccion);
t_list *segment_list_duplicate(t_segments_table *segments_table);
t_segment *segment_duplicate(t_segment *segment);

// DESRTOYERS

void instruccion_destroy(t_instruccion *instruccion);
void lista_instrucciones_destroy(t_list *lista_instrucciones);
void pcontexto_destroy(t_pcontexto *contexto);
void pcontexto_desalojo_destroy(t_pcontexto_desalojo *pcontexto);
void registers_destroy(t_registers *registers);
void segments_table_destroy(t_segments_table *s_table);
void pcb_destroy(t_pcb *pcb);
void info_destroy(t_info *info);
void info_write_destroy(t_info_write *info_write);
void pid_status_destroy(t_pid_status *pid_status);
void data_destroy(t_data *data);

// TO STRING

char *status_code_string(t_status_code sc);

#endif /* TYPES_CONTROLLERS_H_ */