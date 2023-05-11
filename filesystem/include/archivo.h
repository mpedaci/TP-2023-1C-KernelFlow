#ifndef ARCHIVO_H
#define ARCHIVO_H

#include <utils.h> 
#include <commons/bitarray.h> 
#include <fcntl.h> //para la funcion open 
#include <sys/mman.h> //para el mmap
#include <math.h> // para el ceil 
#include <pthread.h>

void initialize_filesystem();
void create_superbloque(); 
void create_bitmap(); 
void create_fcb();
void create_blocks();
int create_file(t_config_filesystem *config ,char *nombre);

#endif /* ARCHIVO_H */
