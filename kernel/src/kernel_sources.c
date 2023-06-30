#include "kernel_sources.h"

// ALL PCB List

void all_pcb_create()
{
    all_pcb = list_create();
}

void all_pcb_destroy()
{
    for (int i = 0; i < list_size(all_pcb); i++)
        pcb_destroy(list_get(all_pcb, i));
    list_destroy(all_pcb);
}

// QUEUES

void queues_create()
{
    queues = malloc(sizeof(t_queues));
    queues->NEW = queue_create();
    queues->READY = queue_create();
    queues->EXEC = queue_create();
    queues->BLOCK = queue_create();
    queues->EXIT = queue_create();
}

t_queue *queue_create()
{
    t_queue *q = malloc(sizeof(t_queue));
    q->queue = list_create();
    pthread_mutex_init(&q->mutex, NULL);
    return q;
}

int find_pcb_index(t_list *cola, uint32_t pid)
{
    for (int i = 0; i < list_size(cola); i++)
        if (((t_pcb *)list_get(cola, i))->pid == pid)
            return i;
    return -1;
}

void queues_destroy()
{
    queue_destroy(queues->NEW);
    queue_destroy(queues->READY);
    queue_destroy(queues->EXEC);
    queue_destroy(queues->BLOCK);
    queue_destroy(queues->EXIT);
    free(queues);
}

void queue_destroy(t_queue *q)
{
    list_destroy(q->queue);
    pthread_mutex_destroy(&q->mutex);
    free(q);
}

// ARCHIVOS

void archives_create()
{
    archivos_abiertos = list_create();
}

void archives_destroy()
{
    for (int i = 0; i < list_size(archivos_abiertos); i++)
        recurso_destroy(list_get(archivos_abiertos, i));
    list_destroy(archivos_abiertos);
}

bool verificar_archivo(char *nombre)
{
    return buscar_archivo(nombre) != NULL;
}

t_recurso *buscar_archivo(char *nombre)
{
    for (int i = 0; i < list_size(archivos_abiertos); i++)
    {
        t_recurso *recurso = list_get(archivos_abiertos, i);
        if (string_equals_ignore_case(recurso->recurso, nombre))
            return recurso;
    }
    return NULL;
}

t_recurso *crear_archivo(char *nombre)
{
    t_recurso *recurso = malloc(sizeof(t_recurso));
    recurso->recurso = string_duplicate(nombre);
    recurso->instancias = 1;
    recurso->lista_bloqueados = list_create();
    pthread_mutex_init(&recurso->mutex, NULL);
    return recurso;
}

t_archivo_abierto *archivo_abierto_create(t_recurso *archivo)
{
    t_archivo_abierto *archivo_abierto = malloc(sizeof(t_archivo_abierto));
    archivo_abierto->archivo = archivo;
    archivo_abierto->puntero = 0;
    return archivo_abierto;
}

t_archivo_abierto *buscar_archivo_abierto(t_pcb *pcb, char *nombre_archivo)
{
    for (int i = 0; i < list_size(pcb->open_files_table); i++)
    {
        t_archivo_abierto *archivo_abierto = list_get(pcb->open_files_table, i);
        if (string_equals_ignore_case(archivo_abierto->archivo->recurso, nombre_archivo))
            return archivo_abierto;
    }
    return NULL;
}

void block_all_archives()
{
    for (int i = 0; i < list_size(archivos_abiertos); i++)
    {
        t_recurso *archivo = list_get(archivos_abiertos, i);
        pthread_mutex_lock(&archivo->mutex);
    }
}

void unblock_all_archives()
{
    for (int i = 0; i < list_size(archivos_abiertos); i++)
    {
        t_recurso *archivo = list_get(archivos_abiertos, i);
        pthread_mutex_unlock(&archivo->mutex);
    }
}

int find_archivo_index(char *nombre_archivo)
{
    for (int i = 0; i < list_size(archivos_abiertos); i++)
    {
        t_recurso *archivo = list_get(archivos_abiertos, i);
        if (string_equals_ignore_case(archivo->recurso, nombre_archivo))
            return i;
    }
    return -1;
}

int find_archivo_abierto_index(t_pcb *pcb, char *nombre_archivo)
{
    for (int i = 0; i < list_size(pcb->open_files_table); i++)
    {
        t_archivo_abierto *archivo_abierto = list_get(pcb->open_files_table, i);
        if (string_equals_ignore_case(archivo_abierto->archivo->recurso, nombre_archivo))
            return i;
    }
    return -1;
}

// RECURSOS

void recursos_create()
{
    recursos = list_create();
    for (int i = 0; i < list_size(config_kernel->recursos); i++)
    {
        t_recurso *recurso = malloc(sizeof(t_recurso));
        recurso->recurso = string_duplicate(list_get(config_kernel->recursos, i));
        recurso->instancias = atoi((char *)list_get(config_kernel->instancias_recursos, i));
        recurso->lista_bloqueados = list_create(); // PCBs Bloqueados
        list_add(recursos, recurso);
        pthread_mutex_init(&recurso->mutex, NULL);
    }
}

void recursos_destroy()
{
    for (int i = 0; i < list_size(recursos); i++)
        recurso_destroy(list_get(recursos, i));
    list_destroy(recursos);
}

void recurso_destroy(t_recurso *recurso)
{
    free(recurso->recurso);
    list_destroy(recurso->lista_bloqueados);
    pthread_mutex_destroy(&recurso->mutex);
    free(recurso);
}

t_pcb *remove_pcb_from_queue_resourse(t_recurso *recurso)
{
    pthread_mutex_lock(&recurso->mutex);
    t_pcb *pcb = list_remove(recurso->lista_bloqueados, 0);
    pthread_mutex_unlock(&recurso->mutex);
    return pcb;
}

t_recurso *find_recurso(char *recurso_solicitado)
{
    for (int i = 0; i < list_size(recursos); i++)
        if (string_equals_ignore_case(recurso_solicitado, ((t_recurso *)list_get(recursos, i))->recurso))
            return list_get(recursos, i);
    return NULL;
}

// TABLAS DE SEGMENTOS

void actualizar_tablas(t_list *tablas_actualizadas)
{
    for (int i = 0; i < list_size(tablas_actualizadas); i++)
    {
        t_segments_table *tabla_actualizada = list_get(tablas_actualizadas, i);
        t_pcb *pcb = list_get(all_pcb, find_pcb_index(all_pcb, tabla_actualizada->pid));
        segments_table_destroy(pcb->segments_table);
        pcb->segments_table = tabla_actualizada;
    }
}