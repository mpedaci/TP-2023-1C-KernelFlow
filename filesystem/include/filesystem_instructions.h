#ifndef FILESYSTEM_INSTRUCTIONS_H
#define FILESYSTEM_INSTRUCTIONS_H

#include <pthread.h>

#include <commons/config.h>
#include <commons/bitarray.h>

#include "filesystem_calculate.h"
#include "filesystem_core.h"
#include "filesystem_vars.h"
#include "filesystem_structs.h"
#include "communication.h"
#include "types.h"
#include "types_controllers.h"

bool create_file(char *nombre);
bool open_file(char *nombre);
bool is_greater_than_max_size_file_allowed(int tamanio_archivo);
bool truncate_file(char *nombre, int nuevo_tamanio);
bool read_file(char *nombre, int puntero_archivo, int cant_bytes, int direccion_fisica);
bool write_file(char *nombre, int puntero_archivo, int cant_bytes, int direccion_fisica);

#endif /* FILESYSTEM_INSTRUCTIONS_H */