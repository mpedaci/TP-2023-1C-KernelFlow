#include <utils.h>
#include <filesystem.h>
#include <commons/bitarray.h>
#include <sys/mman.h>

typedef struct
{
    size_t block_size;
    int block_quantity;
} t_superbloque;

typedef struct
{
    char* data;
} t_block;

void initialize_filesystem();
void create_blocks(char *path_blockfile,int block_quantity,size_t block_size);
void create_block_file(char *path_blockfile,int block_quantity,size_t block_size);
t_superbloque* create_superbloque(); 
void create_bitmap(); 
void create_fcb();
