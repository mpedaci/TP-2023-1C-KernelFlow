#ifndef KERNEL_INSTRUCTIONS_H
#define KERNEL_INSTRUCTIONS_H

#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

#include <commons/string.h>
#include <commons/collections/list.h>

#include "utils.h"
#include "types.h"
#include "kernel_vars.h"
#include "kernel_structs.h"
#include "kernel_core.h"
#include "kernel_sources.h"
#include "kernel_cycle.h"

#include "planner.h"
#include "communication.h"

// EXECUTE RECURSOS

bool execute_wait(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_signal(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_io(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
void *io(void *args);

// EXECUTE KERNEL

bool execute_yield(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_exit(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);

// EXECUTE ARCHIVOS

bool execute_fwrite(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_fread(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_fcreate(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_fopen(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_fclose(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_fseek(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_ftruncate(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
void *file_processing(void *args);

// EXECUTE MEMORIA

bool execute_create_segment(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
bool execute_delete_segment(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_desalojo);
void compactar();

#endif /* KERNEL_INSTRUCTIONS_H */