#ifndef FILESYSTEM_STRUCTS_H
#define FILESYSTEM_STRUCTS_H

#include "types.h"

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

    int retardo_acceso_bloque;
} t_config_filesystem;

typedef struct
{
    t_instruccion *instruccion;
    int client_socket;
} t_inst_client;

#endif /* FILESYSTEM_STRUCTS_H */

