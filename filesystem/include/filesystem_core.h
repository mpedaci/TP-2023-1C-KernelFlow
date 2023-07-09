#ifndef FILESYSTEM_CORE_H
#define FILESYSTEM_CORE_H

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/bitarray.h>

#include "types.h"
#include "filesystem_vars.h"
#include "filesystem_instructions.h"

void end_filesystem();
bool initialize_filesystem();
void create_fcb_dir();
bool load_superbloque();
t_fcb *load_fcb(char *filename);
void update_fcb(t_fcb *fcb);
bool create_fcb(char *filename);
char *get_fcb_path(char *filename);
int get_bitmap_size();
void save_bitmap();
bool load_bitmap();
int get_blocks_size();
void save_blocks();
bool load_blocks();
uint32_t get_cpb();

#endif /* FILESYSTEM_CORE_H */