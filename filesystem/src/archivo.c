
#include<archivo.h>


void initialize_filesystem( t_config_filesystem *config ){

create_fcb(config->path_fcb);
create_superbloque(config->path_superbloque);
create_bitmap(config);
//create_blocks(config->path_bloques);
}

void create_fcb(char *path_fcb){
mkdir(path_fcb,0777);

}


void create_superbloque(char *path_superblock){

    FILE* archivoSuperbloque = fopen(path_superblock, "wb"); // wb xq es archivo binario

    //if(archivoSuperbloque == NULL){
    //    log_info(logger_aux, "NO se pudo abrir el archivo de superbloque");
    //    EXIT_FAILURE;
    //}

    t_superbloque *pSuperbloque= malloc(sizeof(t_superbloque)); //Asigno memoria dinamica al punterosuperbloque
    
    //asigno los dos valores de ejemplo que me da el tp 
    pSuperbloque ->block_size = 64;
    pSuperbloque ->block_count = 30;

    fwrite(pSuperbloque, sizeof(t_superbloque), 1, archivoSuperbloque);

    free(pSuperbloque); 
    fclose(archivoSuperbloque);

}

 void create_bitmap(t_config_filesystem *config ){

    FILE* fileSuperblock= fopen(config->path_superbloque, "rb");
    FILE* fileBitmap = fopen(config->path_bitmap, "wb");

    t_superbloque *pSuperbloque= malloc(sizeof(t_superbloque)); //asigno y creo el puntero con memoria para dsps poder acceder al blockcout

    fread(pSuperbloque,sizeof(t_superbloque),1,fileSuperblock);


    t_bitarray* bitarray = malloc(sizeof(t_bitarray));
    bitarray->size = pSuperbloque->block_count; 
    bitarray->bitarray = malloc(bitarray->size);
    for(int i = 0; i < bitarray->size; i++){
        bitarray->bitarray[i] = '0';
    }
    bitarray->mode = LSB_FIRST;
	fwrite(bitarray, sizeof(t_bitarray), 1, fileBitmap);
    
    free(bitarray);
	fclose(fileBitmap);
	free(pSuperbloque);
	fclose(fileSuperblock);

 }




