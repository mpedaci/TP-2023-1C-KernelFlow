#include<archivo.h>

void initialize_filesystem( t_config_filesystem *config ){

create_fcb(config->path_fcb);
t_superbloque* superbloque = create_superbloque(config->path_superbloque);
create_bitmap(config->path_bitmap,superbloque);
//create_blocks(config->path_bloques);

}

void create_fcb(char *path_fcb){
mkdir(path_fcb,0777);

}

void create_blocks(char *path_blockfile,int block_quantity,size_t block_size) {
    FILE* block_file = fopen(path_blockfile, "rb");
    if (block_file == NULL) {
        log_debug(logger_aux,"No se encontro el archivo de bloques.\n");
        create_block_file(path_blockfile,block_quantity, block_size);
        block_file = fopen(path_blockfile, "rb");
    }

    t_block* blocks = malloc(block_quantity * sizeof(block_size));
    char* all_blocks = mmap(NULL,block_quantity*block_size,PROT_READ,MAP_SHARED,block_file,0);
    
    for (int i = 0; i < block_quantity; i++) {
        blocks[i].data = malloc(block_size);
        memcpy(blocks[i].data, &all_blocks[i * block_size], block_size);
    }

    munmap(all_blocks, block_quantity*block_size);
    close(block_file);
    return blocks;
}

void create_block_file(char *path_blockfile, int block_quantity, size_t block_size) {
    FILE* block_file = fopen(path_blockfile, "w");
    if (block_file == NULL) {
        log_debug(logger_aux,"No se pudo crear el archivo de bloques.\n");
        exit(1);
    }
    
    //Create an empty block/filling a block with 0
    char* empty_block = malloc(block_size);
    memset(empty_block, 0, block_size);

    for (int i = 0; i < block_quantity; i++) {
        fwrite(empty_block, block_size, 1, block_file);
    }

    free(empty_block);
    fclose(block_file);
}


t_superbloque* create_superbloque(char *path_superblock){
    t_config* config = config_create(path_superblock);
    t_superbloque* superbloque = malloc(sizeof(t_superbloque));;

    superbloque->block_size = config_get_long_value(config, "BLOCK_SIZE");
    superbloque->block_quantity = config_get_int_value(config, "BLOCK_COUNT");

    log_info(logger, "Archivo de Superbloque leido correctamente");

    config_destroy(config);

    return superbloque;
}
    

 void create_bitmap(char* path_bitmap, int block_quantity){
    FILE* fileBitmap = fopen(path_bitmap, "wb");

    t_bitarray* bitarray = malloc(sizeof(t_bitarray));
    bitarray->size = block_quantity;
    bitarray->bitarray = malloc(bitarray->size);
    for(int i = 0; i < bitarray->size; i++){
        bitarray->bitarray[i] = '0';
    }
    bitarray->mode = LSB_FIRST;
	fwrite(bitarray, sizeof(t_bitarray), 1, fileBitmap);
    
    free(bitarray);
	fclose(fileBitmap);
 }




