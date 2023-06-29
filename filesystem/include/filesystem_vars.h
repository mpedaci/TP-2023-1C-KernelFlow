#ifndef FILESYSTEM_VARS_H
#define FILESYSTEM_VARS_H

#include <stdint.h>
#include <stdbool.h>
#include <commons/bitarray.h>
#include <commons/log.h>

typedef struct
{
    char *puerto_escucha;
    char *puerto_memoria;
    char *ip_memoria;

    // agrego los paths
    char *path_superbloque;
    char *path_bitmap;
    char *path_bloques;
    char *path_fcb;

    char *retardo_acceso_bloque;

} t_config_filesystem;

extern t_log *logger_main;
extern t_log *logger_aux;

extern t_config_filesystem *config;

extern int memory_socket;

extern void *copy_blocks;
extern t_bitarray *bitmap;

#endif /* FILESYSTEM_VARS_H */