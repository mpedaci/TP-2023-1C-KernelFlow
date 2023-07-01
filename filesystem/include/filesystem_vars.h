#ifndef FILESYSTEM_VARS_H
#define FILESYSTEM_VARS_H

#include <stdint.h>
#include <stdbool.h>
#include <commons/bitarray.h>
#include <commons/log.h>

#include "types.h"
#include "filesystem_structs.h"

extern t_log *logger_main;
extern t_log *logger_aux;

extern t_config_filesystem *config_fs;

extern int memory_socket;

extern void *blocks;

extern t_superbloque *superbloque;
extern t_bitarray *bitmap;

extern int bitmap_file;
extern int blocks_file;

extern pthread_t crear_y_manejar_blocks;

#endif /* FILESYSTEM_VARS_H */