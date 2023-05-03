#include "archivo.h"
//#include <fcntl.h> //para la funcion open 
//#include <sys/mman.h> //para el mmap
//#include <math.h> // para el ceil 


void initialize_filesystem( t_config_filesystem *config ){

create_fcb(config->path_fcb);
create_superbloque(config->path_superbloque);
create_bitmap(config);
create_blocks(config);
}

void create_fcb(char *path_fcb){
    mkdir(path_fcb,0777);

}

void create_superbloque(char *path_superblock){

    t_superbloque *pSuperbloque= malloc(sizeof(t_superbloque));
    //hardcodeado
    pSuperbloque ->block_size = 64;
    pSuperbloque ->block_count = 20;

    int file_superblock = open(path_superblock, O_CREAT | O_RDWR, 0644);

    write(file_superblock, &(pSuperbloque->block_size), sizeof(uint32_t));
    write(file_superblock, &(pSuperbloque->block_count), sizeof(uint32_t));
    //fwrite(&(pSuperbloque->block_size), sizeof(uint32_t), 1, file_superblock);
    
    close(file_superblock);
    free(pSuperbloque); 
}

   /*
      block_size es de 4 bytes 
      blocks es de 4 bytes 
      => tamaño del superbloque es 8 bytes (2*sizeof(uint32_t))


*/ 

void create_bitmap(t_config_filesystem *config ){

    t_superbloque *pSuperbloque= malloc(sizeof(t_superbloque));

    pSuperbloque ->block_size = 64;
    pSuperbloque ->block_count = 20;

    int bitmap_size= ceil(pSuperbloque->block_count / 8 );   //NO PONER 8.0 ROMPE EL CEIL 

    int file_bitmap = open(config->path_bitmap, O_CREAT | O_RDWR, 0644);

    ftruncate(file_bitmap,sizeof(t_superbloque) + bitmap_size);

    void *mmapBitmap = mmap(NULL, sizeof(t_superbloque) + bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_bitmap, 0);
    //en vez de void puede ser char y eso x ahi tmb lo puedo poner como variable global 

    t_bitarray *bitmap = bitarray_create_with_mode(mmapBitmap + sizeof(t_superbloque), bitmap_size, LSB_FIRST);
    bitarray_set_bit(bitmap, false);//pone en 0 c/ bit 

    bitarray_destroy(bitmap);
    
    msync(mmapBitmap, sizeof(t_superbloque) + bitmap_size, MS_SYNC);

    munmap(mmapBitmap, sizeof(t_superbloque) + bitmap_size); //libero memoria mapeada de mmap
    
    free(pSuperbloque);
    close(file_bitmap);

}


void create_blocks(t_config_filesystem *config){

    t_superbloque *pSuperbloque= malloc(sizeof(t_superbloque));

    int file_blocks= open(config->path_bloques,O_CREAT | O_RDWR, 0644);

    int file_block_size = pSuperbloque->block_size * pSuperbloque->block_count;

    ftruncate(file_blocks,file_block_size );

    void* mmapBlocks = mmap(NULL, file_block_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_blocks, 0);

    void* copy_blocks = malloc(file_block_size);
    
    memcpy(copy_blocks,mmapBlocks,file_block_size);

    // Sincronizas los cambios en el archivo de bloques en disco
    sync_blocks(mmapBlocks, file_block_size);

    free(copy_blocks);
    munmap(mmapBlocks, file_block_size);
    close(file_blocks);
}


void sync_blocks(void *mmapBlocks, int file_block_size) {
    if (msync(mmapBlocks, file_block_size, MS_SYNC) == -1) {
        //log_info(logger, "No se pudo sincronizar bloques");
    } else {
        //log_info(logger, "bloques sincronizados");
    }
}