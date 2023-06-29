#ifndef ARCHIVO_H
#define ARCHIVO_H

#include <utils.h>
#include <fcntl.h>    //para la funcion open
#include <sys/mman.h> //para el mmap
#include <math.h>     // para el ceil
#include <pthread.h>
#include <sys/stat.h>

#include <commons/bitarray.h>

#include "filesystem_vars.h"

void initialize_filesystem();
void create_superbloque(char *path_superblock);
void create_bitmap();
void create_fcb(char *path_fcb);
void create_blocks();

int create_file(char *nombre);
int open_file(char *nombre);
int write_file(int puntero_archivo, char *nombre, int cant_bytes, int direccion_fisica);
int read_file(int puntero_archivo, char *nombre, int cant_bytes, int direccion_fisica);
int truncate_file(int nuevo_tamanio, char *nombre);

void sync_blocks(void *mmapBlocks, int file_block_size);
int calculate_additionalBlocks(int nuevo_tamanio, t_fcb *fcb);
int obtener_bloque_libre(t_bitarray *bitmap, int bitmap_size);
int calculate_freeBlocks(int nuevo_tamanio, t_fcb *fcb);
void set_freeBlock_bitmap(t_bitarray *bitmap, int bloque); // Antes era int
int *obtenerPunterosIndirectos(t_fcb *fcb);
int cantidadPunterosIndirectos(int *punterosIndirectos);

void end_filesystem();

#endif /* ARCHIVO_H */