#ifndef FILESYSTEM_CALCULATOR_H
#define FILESYSTEM_CALCULATOR_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <commons/bitarray.h>

#include "filesystem_vars.h"
#include "filesystem_core.h"

#include "types.h"

uint32_t calculate_additionalBlocks(t_fcb *fcb, uint32_t nuevo_tamanio);
int obtener_bloque_libre();
uint32_t calculate_freeBlocks(t_fcb *fcb, uint32_t nuevo_tamanio);
void set_freeBlock_bitmap(int bloque);
uint32_t *obtenerPunterosIndirectos(t_fcb *fcb);
uint32_t cantidadPunterosIndirectos(uint32_t *punterosIndirectos);

#endif /* FILESYSTEM_CALCULATOR_H */