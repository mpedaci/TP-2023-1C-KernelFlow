#ifndef KERNEL_SOURCES_H
#define KERNEL_SOURCES_H

#include <commons/collections/list.h>
#include <commons/string.h>

#include "kernel_vars.h"
#include "kernel_structs.h"
#include "communication.h"
#include "queue_controller.h"

#include "types.h"
#include "types_controllers.h"

// ALL PCB List

void all_pcb_create();
void all_pcb_destroy();

// QUEUES

void queues_create();
t_queue *queue_create();
int find_pcb_index(t_list *cola, uint32_t pid);
void queues_destroy();
void queue_destroy(t_queue *q);

// ARCHIVOS

void archives_create();
void archives_destroy();
bool verificar_archivo(char *nombre);
t_recurso *buscar_archivo(char *nombre);
t_recurso *crear_archivo(char *nombre);
t_archivo_abierto *archivo_abierto_create(t_recurso *archivo);
t_archivo_abierto *buscar_archivo_abierto(t_pcb *pcb, char *nombre_archivo);
void block_all_archives();
void unblock_all_archives();
int find_archivo_index(char *nombre_archivo);
int find_archivo_abierto_index(t_pcb *pcb, char *nombre_archivo);

// RECURSOS

void recursos_create();
void recurso_destroy(t_recurso *recurso);
void recursos_destroy();
t_pcb *remove_pcb_from_queue_resourse(t_recurso *recurso);
t_recurso *find_recurso(char *recurso_solicitado);

// TABLAS DE SEGMENTOS

void actualizar_tablas(t_list *tablas_actualizadas);

#endif /* KERNEL_SOURCES_H */