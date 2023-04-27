#include<archivo.h>

void initialize_filesystem( t_config_filesystem *config ){

create_fcb(config->path_fcb);
t_superbloque* superbloque = create_superbloque(config->path_superbloque);
if(superbloque == NULL){
    free_superbloque(superbloque);
    exit(1);
}
create_bitmap(config->path_bitmap,superbloque->block_quantity);
t_block* bloques =  create_blocks(config->path_bloques, superbloque->block_quantity, superbloque->block_size);
}

void create_fcb(char *path_fcb){
mkdir(path_fcb,0777);

}

t_block* create_blocks(char *path_blockfile,int block_quantity,size_t block_size) {
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
    fclose(block_file);
    return blocks;
}

void create_block_file(char *path_blockfile, int block_quantity, size_t block_size) {
    FILE* block_file = fopen(path_blockfile, "w");
    if (block_file == NULL) {
        log_debug(logger_aux,"No se pudo crear el archivo de bloques.\n");
        exit(1);
    }
    
    // Fill the blocks with zeros
    t_block* empty_blocks = mmap(NULL, block_quantity * block_size, PROT_WRITE, MAP_SHARED, block_file, 0);

    // Fill the blocks with zeros
    for (size_t i = 0; i < block_quantity; i++) {
        empty_blocks[i].data = malloc(block_size);
        memset(empty_blocks[i].data, 0, block_size);
    }

    // Synchronize the memory mapping with the file 
    msync(empty_blocks, block_quantity * block_size, MS_SYNC);

    // Unmap the memory mapping
    munmap(empty_blocks, block_quantity * block_size);
    
    //Close the block_file
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

void free_superbloque(t_superbloque* superbloque){
    free(superbloque->block_quantity);
    free(superbloque->block_size);
    free(superbloque);
}

