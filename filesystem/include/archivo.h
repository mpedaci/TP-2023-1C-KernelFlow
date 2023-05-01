#include <utils.h> 
#include <commons/bitarray.h> 
#include <fcntl.h> //para la funcion open 
#include <sys/mman.h> //para el mmap
#include <math.h> // para el ceil 

typedef struct
{
    uint32_t block_size;
    uint32_t block_count;

} t_superbloque;
typedef struct {

    char* nombre_archivo;
    int tamanio_archivo;
    uint32_t puntero_directo;
    uint32_t puntero_indirecto;

} t_fcb;


void initialize_filesystem();
void create_superbloque(); 
void create_bitmap(); 
void create_fcb();
void create_blocks();