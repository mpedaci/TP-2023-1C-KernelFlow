#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

#include <commons/string.h>
#include <commons/collections/list.h>

#include "types.h"
#include "kernel_vars.h"
#include "kernel_structs.h"
#include "kernel_core.h"

#include "planner.h"
#include "communication.h"


bool execute_create_segment(t_instruccion *instruccion, t_pcb *pcb);
bool execute_delete_segment(t_instruccion *instruccion, t_pcb *pcb);
bool execute_wait(char *recurso_solicitado, t_pcb *pcb);
bool execute_signal(char *recurso_solicitado, t_pcb *pcb);
void execute_io(int tiempo, t_pcb *pcb);
void execute_exit(t_pcb *pcb, t_status_code sc);
void execute_to_ready(t_pcb *pcb);
void execute_fread(t_instruccion *instruccion, t_pcb *pcb);
void execute_fwrite(t_instruccion *instruccion, t_pcb *pcb);
void execute_ftruncate(t_instruccion* instruccion, t_pcb *pcb);

void compactar();
void actualizar_tablas(t_list *tablas_actualizadas);

void *io(void *tiempo);

t_recurso *find_recurso(char *recurso_solicitado);
int find_pcb_index(t_list *cola, uint32_t pid);
t_pcb *find_pcb(uint32_t pid);

char *get_status_code_string(t_status_code sc);

#endif /* INSTRUCCIONES_H */