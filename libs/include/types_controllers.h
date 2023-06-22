#ifndef TYPES_CONTROLLERS_H_
#define TYPES_CONTROLLERS_H_

#include <stdlib.h>
#include <stdint.h>

#include <commons/temporal.h>
#include <commons/collections/list.h>

#include "types.h"

void instruccion_destroy(t_instruccion *instruccion);
void lista_instrucciones_destroy(t_list *lista_instrucciones);
void pcontexto_destroy(t_pcontexto *contexto);
void pcontexto_desalojo_destroy(t_pcontexto_desalojo *pcontexto);
void registers_destroy(t_registers *registers);
void segments_table_destroy(t_segments_table *s_table);
void destroy_pcb(t_pcb *pcb);
void info_destroy(t_info *info);
void info_write_destroy(t_info_write *info_write);

#endif /* TYPES_CONTROLLERS_H_ */