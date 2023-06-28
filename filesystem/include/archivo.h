#ifndef ARCHIVO_H
#define ARCHIVO_H

#include <utils.h> 
#include <commons/bitarray.h> 
#include <fcntl.h> //para la funcion open 
#include <sys/mman.h> //para el mmap
#include <math.h> // para el ceil 
#include <pthread.h>

void initialize_filesystem( t_config_filesystem *config, t_log *logger_main);
void create_superbloque(char *path_superblock); 
void create_bitmap(t_config_filesystem *config ); 
void create_fcb(char *path_fcb);
void create_blocks(t_config_filesystem *config);

int create_file(t_config_filesystem *config ,char *nombre, t_log *logger_main);
int open_file(t_config_filesystem *config ,char *nombre, t_log *logger_main);
int write_file(t_config_filesystem *config, int puntero_archivo, char *nombre, int cant_bytes, void *direccion_fisica, t_log *logger_main);
int read_file(t_config_filesystem *config, int puntero_archivo, char *nombre, int cant_bytes, void *direccion_fisica, t_log *logger_main);
int truncate_file(t_config_filesystem *config, int nuevo_tamanio, char *nombre, t_log *logger_aux);

void sync_blocks(void *mmapBlocks, int file_block_size);
int calculate_additionalBlocks(int nuevo_tamanio, t_fcb *fcb);
int obtener_bloque_libre(t_bitarray *bitmap, int bitmap_size, t_log *logger_main);
int get_freeBlock(t_bitarray *bitmap, int bitmap_size);
int calculate_freeBlocks(int nuevo_tamanio, t_fcb *fcb);
void set_freeBlock_bitmap(t_bitarray *bitmap, int bloque); //Antes era int
int* obtenerPunterosIndirectos(t_fcb* fcb,t_log* logger_main);
int cantidadPunterosIndirectos(int* punterosIndirectos);

void end_filesystem();

#endif /* ARCHIVO_H */