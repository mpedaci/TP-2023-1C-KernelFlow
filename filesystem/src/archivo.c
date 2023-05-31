#include "archivo.h"
// #include <fcntl.h> //para la funcion open
// #include <sys/mman.h> //para el mmap
// #include <math.h> // para el ceil

t_superbloque *pSuperbloque;
void *copy_blocks;
t_fcb *fcb;
t_bitarray *bitmap;
int bitmap_size;

void initialize_filesystem(t_config_filesystem *config)
{

    create_fcb(config->path_fcb);

    create_superbloque(config->path_superbloque);
    create_bitmap(config);
    pthread_t crear_y_manejar_blocks;
    pthread_create(&crear_y_manejar_blocks, NULL, (void *)create_blocks, (void *)config);
    pthread_detach(crear_y_manejar_blocks);

    // PARA PROBAR SI FUNCIONA DESCOMENTAR ESTAS FUNCIONES DE ACA !!!
    // son ejemplos:
    // create_file(config,"pepe");
    // truncate_file(config,200,"pepe");
}

void create_fcb(char *path_fcb)
{
    mkdir(path_fcb, 0777);
}

void create_superbloque(char *path_superblock)
{

    pSuperbloque = malloc(sizeof(t_superbloque));
    // hardcodeado
    pSuperbloque->block_size = 64;
    pSuperbloque->block_count = 20;

    int file_superblock = open(path_superblock, O_CREAT | O_RDWR, 0644);

    write(file_superblock, &(pSuperbloque->block_size), sizeof(uint32_t));
    write(file_superblock, &(pSuperbloque->block_count), sizeof(uint32_t));
    // fwrite(&(pSuperbloque->block_size), sizeof(uint32_t), 1, file_superblock);

    close(file_superblock);
    // free(pSuperbloque);
}

/*
   block_size es de 4 bytes
   blocks es de 4 bytes
   => tamaño del superbloque es 8 bytes (2*sizeof(uint32_t))



*/

void create_bitmap(t_config_filesystem *config)
{

    int bitmap_size = ceil(pSuperbloque->block_count / 8); // NO PONER 8.0 ROMPE EL CEIL

    int file_bitmap = open(config->path_bitmap, O_CREAT | O_RDWR, 0644);

    ftruncate(file_bitmap, sizeof(t_superbloque) + bitmap_size);

    void *mmapBitmap = mmap(NULL, sizeof(t_superbloque) + bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_bitmap, 0);
    // en vez de void puede ser char y eso x ahi tmb lo puedo poner como variable global

    bitmap = bitarray_create_with_mode(mmapBitmap + sizeof(t_superbloque), bitmap_size, LSB_FIRST);
    bitarray_set_bit(bitmap, false); // pone en 0 c/ bit

    bitarray_destroy(bitmap);

    msync(mmapBitmap, sizeof(t_superbloque) + bitmap_size, MS_SYNC);

    munmap(mmapBitmap, sizeof(t_superbloque) + bitmap_size); // libero memoria mapeada de mmap

    // free(pSuperbloque);
    close(file_bitmap);
}

void create_blocks(t_config_filesystem *config)
{

    // t_superbloque *pSuperbloque= malloc(sizeof(t_superbloque));

    int file_blocks = open(config->path_bloques, O_CREAT | O_RDWR, 0644);

    int file_block_size = pSuperbloque->block_size * pSuperbloque->block_count;

    ftruncate(file_blocks, file_block_size);

    void *mmapBlocks = mmap(NULL, file_block_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_blocks, 0);

    copy_blocks = malloc(file_block_size);
    // semaforos
    memcpy(copy_blocks, mmapBlocks, file_block_size);

    while (1)
    {
        // Sincronizas los cambios en el archivo de bloques en disco
        sleep(config->retardo_acceso_bloque);
        memcpy(mmapBlocks, copy_blocks, file_block_size);
        sync_blocks(mmapBlocks, file_block_size);
    }

    // free(copy_blocks);
    munmap(mmapBlocks, file_block_size);
    close(file_blocks);
}

void sync_blocks(void *mmapBlocks, int file_block_size)
{
    if (msync(mmapBlocks, file_block_size, MS_SYNC) == -1)
    {
        // log_info(logger, "No se pudo sincronizar bloques");
    }
    else
    {
        // log_info(logger, "bloques sincronizados");
    }
}

// como me dice q devuelve siempre OK entonces es una funcion q devuelve algo, es decir no puede ser void

int create_file(t_config_filesystem *config, char *nombre)
{

    fcb = malloc(sizeof(t_fcb));
    fcb->nombre_archivo = malloc(strlen(nombre) + 1);

    strcpy(fcb->nombre_archivo, nombre);
    fcb->tamanio_archivo = 0;
    fcb->puntero_directo = 0;
    fcb->puntero_indirecto = 0;
    char *ruta_Fcb = malloc(strlen(config->path_fcb) + strlen(nombre) + 2);
    strcpy(ruta_Fcb, config->path_fcb);
    strcat(ruta_Fcb, "/");
    strcat(ruta_Fcb, nombre);
    int file_fcb = open(ruta_Fcb, O_CREAT | O_RDWR, 0644);
    // int file_fcb = open(config->path_fcb, O_WRONLY |O_CREAT| O_APPEND );
    write(file_fcb, &fcb->nombre_archivo, strlen(nombre) + 1);
    write(file_fcb, &(fcb->tamanio_archivo), sizeof(int));
    write(file_fcb, &fcb->puntero_directo, sizeof(uint32_t));
    write(file_fcb, &fcb->puntero_indirecto, sizeof(uint32_t));
    close(file_fcb);
    free(ruta_Fcb);
    free(fcb->nombre_archivo);

    // free(fcb);
    return 0; // devuelvo 0 siempre porque es exitoso, siempre se puede crear el archivo
    // return OK   podria llegar a implementar el OK en vez del 0 , preguntar!!!
}

void truncate_file(t_config_filesystem *config, int nuevo_tamanio, char *nombre)
{

    fcb = malloc(sizeof(t_fcb));
    // fcb->nombre_archivo=malloc(strlen(nombre)+1);  //  a ver si agregando esto se soluciona

    char *ruta_Fcb = malloc(strlen(config->path_fcb) + strlen(nombre) + 2);
    strcpy(ruta_Fcb, config->path_fcb);
    strcat(ruta_Fcb, "/");
    strcat(ruta_Fcb, nombre);
    int file_fcb = open(ruta_Fcb, O_RDWR);
    // strcpy(fcb->nombre_archivo,nombre);

    // read(file_fcb, &fcb, sizeof(t_fcb));  probar si esto es igual a leer cada uno por separado

    read(file_fcb, &(fcb->tamanio_archivo), sizeof(int));
    printf("tamanio actual archivo %d  \n", fcb->tamanio_archivo);
    read(file_fcb, &fcb->puntero_directo, sizeof(uint32_t));
    read(file_fcb, &fcb->puntero_indirecto, sizeof(uint32_t));
    read(file_fcb, &fcb->nombre_archivo, sizeof(strlen(nombre) + 1));

    if (nuevo_tamanio > fcb->tamanio_archivo)
    { // CASO EXPANDIR TAMAÑO

        int bloques_Adicionales = calculate_additionalBlocks(nuevo_tamanio, fcb);
        // printf("Se requieren %d bloques adicionales \n", bloques_Adicionales);

        if (bloques_Adicionales > 0)
        {
            int i;
            int bloque_libre;
            for (i = 0; i < bloques_Adicionales; i++)
            {
                bloque_libre = get_freeBlock(bitmap, bitmap_size);

                if (i == 0)
                {
                    bloque_libre = get_freeBlock(bitmap, bitmap_size);
                    fcb->puntero_directo = bloque_libre;
                    bloques_Adicionales--;
                }

                asignar_bloque_indirecto(config, fcb->puntero_indirecto, bloque_libre);
            }
        }
        fcb->tamanio_archivo = nuevo_tamanio;
        write(file_fcb, &fcb, sizeof(int)); // escribo todo ya fue, asi se actualiza todo x las dudas
        // write(file_fcb,&fcb->tamanio_archivo,sizeof(int));  sino esta esta opcion escribiri solo el tam, vemoss
    }
    else if (nuevo_tamanio < fcb->tamanio_archivo)
    { // CASO ACHICHAR TAMAÑO

        int bloques_a_liberar = calculate_freeBlocks(nuevo_tamanio, fcb);
        if (bloques_a_liberar > 0)
        {
            int i = 0;
            int bloque_a_liberar;
            for (int i = bloques_a_liberar - 1; i >= 0; i--)
            { // voy decrementando los bloques q libero

                bloque_a_liberar = fcb->puntero_directo;
                set_freeBlock_bitmap(bitmap, bloque_a_liberar);
            }
        }
        fcb->tamanio_archivo = nuevo_tamanio;
        write(file_fcb, &fcb, sizeof(int)); // escribo todo ya fue, asi se actualiza todo x las dudas
    }

    free(ruta_Fcb);
    free(fcb);
    close(file_fcb);
}

//---------------------funciones aux para expandir el tamaño----------------------------------------//

int calculate_additionalBlocks(int nuevo_tamanio, t_fcb *fcb)
{

    // VER EL TEMA DEL CEIL XQ ROMPE(es mejor esta forma)
    // int bloques_actuales = ceil((double)fcb->tamanio_archivo/ pSuperbloque->block_size);
    // int bloques_nuevos = ceil((double)nuevo_tamanio / pSuperbloque->block_size);

    // mientras tanto se calcula de esta manera
    printf("tamaño archivo: %d \n", fcb->tamanio_archivo);
    printf("tamaño block size del superbloque :  %d \n", pSuperbloque->block_size);

    int bloques_actuales = fcb->tamanio_archivo / pSuperbloque->block_size;
    if (fcb->tamanio_archivo % pSuperbloque->block_size > 0)
    {
        bloques_actuales++;
    }

    int bloques_nuevos = nuevo_tamanio / pSuperbloque->block_size;
    if (fcb->tamanio_archivo % pSuperbloque->block_size > 0)
    {
        bloques_nuevos++;
    }

    int bloquesAdicionales = bloques_nuevos - bloques_actuales;
    return bloquesAdicionales;
}

int get_freeBlock(t_bitarray *bitmap, int bitmap_size)
{
    int bloque_libre = -1;

    bitmap_size = bitarray_get_max_bit(bitmap);
    for (int i = 0; i < pSuperbloque->block_count; i++)
    {

        if (!bitarray_test_bit(bitmap, i))
        {                                // el bit en la pos i del bitmap es 0 osea esta libre =>
            bitarray_set_bit(bitmap, i); // lo seteo como ocupado (1)
            bloque_libre = i;
        }
    }
    return bloque_libre;
}

void asignar_bloque_indirecto(t_config_filesystem *config, uint32_t bloque_puntero_indirecto, int bloque)
{

    int file_blocks = open(config->path_bloques, O_RDWR, 0644);
    // me muevo a la posicion donde esta en el fileblocks
    lseek(file_blocks, bloque_puntero_indirecto * pSuperbloque->block_size, SEEK_SET);
    write(file_blocks, &bloque, sizeof(uint32_t));
    close(file_blocks);
}

//-------------------------funciones aux para reducir tamaño ----------------------------------//

int calculate_freeBlocks(int nuevo_tamanio, t_fcb *fcb)
{
    // VER EL TEMA DEL CEIL XQ ROMPE(es mejor esta forma)
    // int bloques_actuales = ceil((double)fcb->tamanio_archivo/ pSuperbloque->block_size);
    // int bloques_nuevos = ceil((double)nuevo_tamanio / pSuperbloque->block_size);

    // mientras tanto se calcula de esta manera
    int bloques_actuales = fcb->tamanio_archivo / pSuperbloque->block_size;
    if (fcb->tamanio_archivo % pSuperbloque->block_size > 0)
    {
        bloques_actuales++;
    }

    int bloques_nuevos = nuevo_tamanio / pSuperbloque->block_size;
    if (fcb->tamanio_archivo % pSuperbloque->block_size > 0)
    {
        bloques_nuevos++;
    }

    int bloques_libres = bloques_actuales - bloques_nuevos;
    return bloques_libres;
}

int set_freeBlock_bitmap(t_bitarray *bitmap, int bloque)
{
    bitarray_clean_bit(bitmap, bloque);
}
