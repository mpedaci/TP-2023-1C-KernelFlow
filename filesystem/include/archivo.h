
#include <utils.h> 
#include <commons/bitarray.h> 
typedef struct
{
    size_t block_size;
    int block_count;
} t_superbloque;



void initialize_filesystem();
void create_superbloque(); 
void create_bitmap(); 
void create_fcb();
