#include "archivo.h"

t_superbloque *pSuperbloque;
void *copy_blocks;
t_bitarray *bitmap;
int bitmap_size;

void end_filesystem()
{
    free(copy_blocks);
    free(bitmap);
    free(pSuperbloque);
}

void initialize_filesystem()
{ 

    create_fcb(config->path_fcb);

    create_superbloque(config->path_superbloque);
    create_bitmap(config);

    pthread_t crear_y_manejar_blocks;
    pthread_create(&crear_y_manejar_blocks, NULL, (void *)create_blocks, (void *)config);
    pthread_detach(crear_y_manejar_blocks);

    /*---------------PARA PROBAR SI FUNCIONA DESCOMENTAR ESTAS FUNCIONES DE ACA-----------*/

    // Ejemplos crear archivos:

     //create_file("pepe");
     //open_file("pepe");
     //truncate_file( 400, "pepe");
     //write_file( 200, "pepe", 120, 3456789);
     //read_file( 200, "pepe", 120, 3456789); 
     //create_file("juanma");
     //create_file("meli");

    // Ejemplos abrir archivos:

     //open_file("pepe");
     //open_file("meli");

    // Ejemplos truncar el mismo archivo con diferentes tamaños:

     //truncate_file(300, "meli");
     //truncate_file(300, "juanma");
     //truncate_file(150,"meli");
     //truncate_file(200,"pepe");

    // Ejemplos escribir archivo:
    //write_file( 35, "meli", 120, 3456789);
    //write_file( 35, "juanma", 120, 3456789);

    // Ejemplos leer archivo:
    //read_file(35, "meli", 120, 3456789);
    //read_file( 40, "juanma", 50, 3456789);
    //read_file( 40, "pepe", 50, 3456789);
     
}
/*---------------------------------Funciones para levantar las estructuras----------------------------*/

void create_fcb(char *path_fcb)
{
    mkdir(path_fcb, 0777);
}

void create_superbloque(char *path_superblock)
{

    pSuperbloque = malloc(sizeof(t_superbloque));
    pSuperbloque->block_size = 64;
    pSuperbloque->block_count = 20;

    int file_superblock = open(path_superblock, O_CREAT | O_RDWR, 0644);

    write(file_superblock, &(pSuperbloque->block_size), sizeof(uint32_t));
    write(file_superblock, &(pSuperbloque->block_count), sizeof(uint32_t));

    close(file_superblock);
}

void create_bitmap()
{
    bitmap_size = ((pSuperbloque->block_count + 7) / 8);
    log_info(logger_main, "Path bitmap: %s", config->path_bitmap);

    int file_bitmap = open(config->path_bitmap, O_CREAT | O_RDWR, 0644);

    ftruncate(file_bitmap, sizeof(t_superbloque) + bitmap_size);

    void *mmapBitmap = mmap(NULL, sizeof(t_superbloque) + bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_bitmap, 0);

    bitmap = bitarray_create_with_mode(mmapBitmap + sizeof(t_superbloque), bitmap_size, LSB_FIRST);

    for (int i = 0; i < pSuperbloque->block_count; i++)
    {
        bitarray_clean_bit(bitmap, i);
    }

    // test_bitmap_zeros(bitmap); para testear que el bitmap esta en 0 todos
    msync(mmapBitmap, sizeof(t_superbloque) + bitmap_size, MS_SYNC);
    munmap(mmapBitmap, sizeof(t_superbloque) + bitmap_size); // libero memoria mapeada de mmap
    close(file_bitmap);
}

void test_bitmap_zeros(t_bitarray *bitmap)
{
    int bitmap_size = bitarray_get_max_bit(bitmap);

    for (int i = 0; i < bitmap_size; i++)
    {
        if (bitarray_test_bit(bitmap, i))
        {
            printf("El bit en la posición %d no está en 0\n", i);
            return;
        }
    }
    // printf("Todos los bits del bitmap están en 0\n");
}

void create_blocks()
{

    int file_blocks = open(config->path_bloques, O_CREAT | O_RDWR, 0644);
    int file_block_size = pSuperbloque->block_size * pSuperbloque->block_count;

    ftruncate(file_blocks, file_block_size);
    void *mmapBlocks = mmap(NULL, file_block_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_blocks, 0);

    copy_blocks = malloc(file_block_size);
    memcpy(copy_blocks, mmapBlocks, file_block_size);

    while (1)
    {
        // Sincroniza los cambios en el archivo de bloques en disco
        sleep(atoi(config->retardo_acceso_bloque));
        memcpy(mmapBlocks, copy_blocks, file_block_size);
        sync_blocks(mmapBlocks, file_block_size);
    }

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

/*----------------------------Funciones para operar con los archivos----------------------------------*/

int create_file(char *nombre)
{

    char *ruta_Fcb = malloc(strlen(config->path_fcb) + strlen(nombre) + 2);
    strcpy(ruta_Fcb, config->path_fcb);
    strcat(ruta_Fcb, "/");
    strcat(ruta_Fcb, nombre);
    FILE *file_fcb = fopen(ruta_Fcb, "w");
    t_fcb *fcb = malloc(sizeof(t_fcb));
    fcb->nombre_archivo = malloc(strlen(nombre) + 1);

    strcpy(fcb->nombre_archivo, nombre);
    fcb->tamanio_archivo = 0;
    fcb->puntero_directo = 0;
    fcb->puntero_indirecto = 0;

    fwrite(fcb, sizeof(t_fcb), 1, file_fcb);

    fclose(file_fcb);
    log_info(logger_main, "Crear archivo: %s", fcb->nombre_archivo);

    free(ruta_Fcb);
    free(fcb->nombre_archivo); 
    free(fcb);

    return 0; 
}

int open_file(char *nombre)
{
    char* ruta_Fcb=malloc(strlen(config->path_fcb)+strlen(nombre)+2);
    strcpy(ruta_Fcb,config->path_fcb);
    strcat(ruta_Fcb,"/");
    strcat(ruta_Fcb,nombre);
    FILE* file_fcb = fopen(ruta_Fcb,"r");

    t_fcb*fcb = malloc(sizeof(t_fcb));

    if(file_fcb!=NULL){

        fread(fcb, sizeof(t_fcb), 1, file_fcb);
        fcb->nombre_archivo=malloc(strlen(nombre)+1);
        strcpy(fcb->nombre_archivo, nombre);
        log_info(logger_main,"Abrir archivo:  %s", fcb->nombre_archivo);
        fclose(file_fcb);
        free(ruta_Fcb);
        free(fcb->nombre_archivo); 
        free(fcb); 
        return 0; 

    }else{
        printf("No existe el archivo: %s \n", file_fcb );
        free(ruta_Fcb);
        free(fcb); 
        return 1; 
    }

}

int truncate_file(int nuevo_tamanio, char *nombre)
{
    char *ruta_Fcb = malloc(strlen(config->path_fcb) + strlen(nombre) + 2);
    strcpy(ruta_Fcb, config->path_fcb);
    strcat(ruta_Fcb, "/");
    strcat(ruta_Fcb, nombre);
    FILE *file_fcb = fopen(ruta_Fcb, "r");
    if (file_fcb != NULL)
    {
        t_fcb *fcb = malloc(sizeof(t_fcb));
        fread(fcb, sizeof(t_fcb), 1, file_fcb);
        fcb->nombre_archivo=malloc(strlen(nombre)+1);
        strcpy(fcb->nombre_archivo, nombre);

        int *punterosIndirectos;
        if (fcb->puntero_indirecto != 0)
        {
            punterosIndirectos = obtenerPunterosIndirectos(fcb);
        }
        if (nuevo_tamanio > fcb->tamanio_archivo)
        { // CASO EXPANDIR TAMAÑO
            int bloques_Adicionales = calculate_additionalBlocks(nuevo_tamanio, fcb);
            // printf("Se requieren %d bloques adicionales \n", bloques_Adicionales); // PARA VER

            if (bloques_Adicionales > 0)
            {
                int i;
                int bloque_libre;
                // entonces hay un puntero indirecto
                int bloque_libre_puntero_indirecto = 0;
                if (fcb->puntero_indirecto == 0)
                {
                    bloque_libre = obtener_bloque_libre(bitmap, bitmap_size); // bloque directo
                    fcb->puntero_directo = bloque_libre;
                    if (bloques_Adicionales > 1)
                    {
                        bloque_libre_puntero_indirecto = obtener_bloque_libre(bitmap, bitmap_size);
                        fcb->puntero_indirecto = bloque_libre_puntero_indirecto;
                        punterosIndirectos = malloc((bloques_Adicionales - 1) * sizeof(int));

                        for (i = 0; i < bloques_Adicionales - 1; i++)
                        {
                            bloque_libre = obtener_bloque_libre(bitmap, bitmap_size);
                            punterosIndirectos[i] = bloque_libre; // asigno el bloque libre a los punteros q faltan para expandir el tamaño
                        }
                    }
                }
                else
                {
                    bloque_libre_puntero_indirecto = fcb->puntero_indirecto;
                    int z = 0;

                    for (i = 0; i < bloques_Adicionales; i++)
                    {
                        bloque_libre = obtener_bloque_libre(bitmap, bitmap_size);
                        while (punterosIndirectos[z] != 0 && punterosIndirectos[z] != 9999)
                        {
                            z++;
                        }
                        punterosIndirectos[z] = bloque_libre; // asigno el bloque libre a los punteros q faltan para expandir el tamaño
                    }
                }
                int cantPunterosIndirectos = 0;
                cantPunterosIndirectos = cantidadPunterosIndirectos(punterosIndirectos);
                memcpy(copy_blocks + bloque_libre_puntero_indirecto * pSuperbloque->block_size, punterosIndirectos, sizeof(int) * cantPunterosIndirectos);
            }

            fcb->tamanio_archivo = nuevo_tamanio; // cambio el tamanio al nuevo
        }
        else if (nuevo_tamanio < fcb->tamanio_archivo)
        { // CASO ACHICHAR TAMAÑO

            int bloques_a_liberar = calculate_freeBlocks(nuevo_tamanio, fcb);
            // printf("Se requieren %d bloques a liberar \n", bloques_a_liberar); //PARA VER

            if (bloques_a_liberar > 0 && nuevo_tamanio != 0)
            {
                int bloque_a_liberar;
                int cantPunterosIndirectos = cantidadPunterosIndirectos(punterosIndirectos);
                if (cantPunterosIndirectos == bloques_a_liberar)
                {
                    int bloque_a_eliminar_pi = fcb->puntero_indirecto;
                    set_freeBlock_bitmap(bitmap, bloque_a_eliminar_pi);
                }

                for (int i = bloques_a_liberar; i > 0; i--)
                { // voy decrementando los bloques q libero
                    bloque_a_liberar = punterosIndirectos[cantPunterosIndirectos - i];
                    set_freeBlock_bitmap(bitmap, bloque_a_liberar);
                    punterosIndirectos[cantPunterosIndirectos - i] = 0; // Asigno valor inválido al puntero liberado
                }
                if (cantPunterosIndirectos > bloques_a_liberar)
                {
                    memcpy(copy_blocks + fcb->puntero_indirecto * pSuperbloque->block_size, punterosIndirectos, sizeof(int) * cantPunterosIndirectos);
                }
            }
            else if (nuevo_tamanio == 0)
            {
                if (fcb->puntero_indirecto != 0)
                {
                    for (int i = 0; i < bloques_a_liberar; i++)
                    {
                        int bloque_a_eliminar = punterosIndirectos[i];
                        set_freeBlock_bitmap(bitmap, bloque_a_eliminar);
                    }
                    int bloque_a_eliminar_pi = fcb->puntero_indirecto;
                    set_freeBlock_bitmap(bitmap, bloque_a_eliminar_pi);
                }

                int bloque_a_eliminar_pd = fcb->puntero_directo;
                set_freeBlock_bitmap(bitmap, bloque_a_eliminar_pd);
            }
        }

        fcb->tamanio_archivo = nuevo_tamanio;
        fclose(file_fcb);
        FILE *file_fcb_escritura = fopen(ruta_Fcb, "w");
        fwrite(fcb, sizeof(t_fcb), 1, file_fcb_escritura);
        fclose(file_fcb_escritura);
        log_info(logger_main, "Truncar archivo:  %s - Tamaño: %d ", fcb->nombre_archivo, fcb->tamanio_archivo);
        free(punterosIndirectos); 
        free(fcb->nombre_archivo);
        free(fcb);
        free(ruta_Fcb);
        //}
        return 0;
    }
    return 1; // FAIL
}

//---------------------funciones aux para expandir el tamaño----------------------------------------//

int calculate_additionalBlocks(int nuevo_tamanio, t_fcb *fcb)
{
    int cant_bloques_actual = fcb->tamanio_archivo / pSuperbloque->block_size;
    if (fcb->tamanio_archivo % pSuperbloque->block_size != 0)
    {
        cant_bloques_actual += 1;
    }

    int cant_bloques_nuevo = nuevo_tamanio / pSuperbloque->block_size;
    if (nuevo_tamanio % pSuperbloque->block_size != 0)
    {
        cant_bloques_nuevo += 1;
    }

    int bloques_adicionales = cant_bloques_nuevo - cant_bloques_actual;
    return bloques_adicionales;
}

int obtener_bloque_libre(t_bitarray *bitmap, int bitmap_size)
{ // obtengo el bloque libre y asigno como ocupado en el bitmap

    int bloque_libre;
    for (int i = 0; i < pSuperbloque->block_count; i++)
    {
        if (!bitarray_test_bit(bitmap, i))
        {                                // el bit en la pos i del bitmap es 0 osea esta libre =>
            bitarray_set_bit(bitmap, i); // lo seteo como ocupado (1)                      
            log_info(logger_main, "Acceso a Bitmap - Bloque: %d - Estado: 0 LIBRE", i);
            bloque_libre = i;
            break; // nose si va
        }
        else
        {
            log_info(logger_main, "Acceso a Bitmap - Bloque: %d - Estado: 1 OCUPADO", i);
        }
    }
    return bloque_libre;
}

//-------------------------funciones aux para reducir tamaño ----------------------------------//

int calculate_freeBlocks(int nuevo_tamanio, t_fcb *fcb)
{

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

void set_freeBlock_bitmap(t_bitarray *bitmap, int bloque)
{
    bitarray_clean_bit(bitmap, bloque);
}

int *obtenerPunterosIndirectos(t_fcb *fcb)
{
    int *punterosIndirectosActuales = malloc(pSuperbloque->block_size);
    int j = 0;
    while (j * sizeof(int) < pSuperbloque->block_size)
    { 
        punterosIndirectosActuales[j] = 9999;
        j++;
    }
    if (fcb->puntero_indirecto != 0)
    {
        int z = 0;
        memcpy(&punterosIndirectosActuales[z], copy_blocks + (pSuperbloque->block_size * fcb->puntero_indirecto) + (z * sizeof(int)), sizeof(int)); 
        // log_info(logger_aux," puntero indirecto: %d",punterosIndirectosActuales[z]); //PARA VER
        z++;
        while (punterosIndirectosActuales[z - 1] != 0 && (z) * sizeof(int) < pSuperbloque->block_size)
        {
            memcpy(&punterosIndirectosActuales[z], copy_blocks + (pSuperbloque->block_size * fcb->puntero_indirecto) + (z * sizeof(int)), sizeof(int)); 
            // log_info(logger_aux,"puntero indirecto %d",punterosIndirectosActuales[z]); //PARA VER
            z++;
        }
        return punterosIndirectosActuales;
    }
    return NULL;
}

int cantidadPunterosIndirectos(int *punterosIndirectos)
{
    int contador = 0;
    for (int i = 0; punterosIndirectos[i] != 0 && punterosIndirectos[i] != 9999; i++)
    {
        // printf("punterosIndirectos: %d\n",punterosIndirectos[i]);  //PARA VER
        contador++;
    }
    return contador;
}

/**.....................................................................................................*/

int read_file(int puntero_archivo, char *nombre, int cant_bytes, int direccion_fisica)
{

    int cant_bytes_inicial = cant_bytes;
    char *ruta_Fcb = malloc(strlen(config->path_fcb) + strlen(nombre) + 2);
    strcpy(ruta_Fcb, config->path_fcb);
    strcat(ruta_Fcb, "/");
    strcat(ruta_Fcb, nombre);
    FILE *file_fcb = fopen(ruta_Fcb, "r");
    if (file_fcb == NULL)
    {
        return 1;
    }

    t_fcb *fcb = malloc(sizeof(t_fcb));
    fread(fcb, sizeof(t_fcb), 1, file_fcb);
    fcb->nombre_archivo=malloc(strlen(nombre)+1);
    strcpy(fcb->nombre_archivo, nombre);

    void *stream = malloc(cant_bytes);

    int numero_de_bloque_inicial = puntero_archivo / pSuperbloque->block_size;
    int cantidad_de_bloques_a_leer = cant_bytes / pSuperbloque->block_size;
    int offset_aux = cant_bytes - cantidad_de_bloques_a_leer * pSuperbloque->block_size;                                           // desde donde empiezo a escribir en el bloque
    int offset_dentro_bloque_pArchivo = puntero_archivo - (puntero_archivo / pSuperbloque->block_size) * pSuperbloque->block_size; // desde donde empiezo a escribir en el bloque
    int cant_bytes_disponibles = pSuperbloque->block_size - offset_dentro_bloque_pArchivo;

    if (offset_aux > cant_bytes_disponibles)
    {
        cantidad_de_bloques_a_leer++;
    }

    if (cant_bytes % pSuperbloque->block_size > 0)
    {
        cantidad_de_bloques_a_leer++;

    }

    int *punterosIndirectos;
    int cant_bytes_a_leer = 0;

    if (fcb->puntero_indirecto != 0)
    {
        punterosIndirectos = obtenerPunterosIndirectos(fcb);
    }

    if (numero_de_bloque_inicial == 0)//PARA VER
    {
        if (cantidad_de_bloques_a_leer == 1)
        {
            memcpy(stream, copy_blocks + fcb->puntero_directo * pSuperbloque->block_size + offset_dentro_bloque_pArchivo, cant_bytes);
            log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, 0, fcb->puntero_directo);
        }
        else
        {
            memcpy(stream, copy_blocks + fcb->puntero_directo * pSuperbloque->block_size + offset_dentro_bloque_pArchivo, cant_bytes_disponibles);
            log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, 0, fcb->puntero_directo);

            //printf("en el puntero directo: %d \n", fcb->puntero_directo); //PARA VER
            //printf("LEO Contenido de copy_blocks bloque puntero directo: %s \n", stream); //PARA VER
            cant_bytes = cant_bytes - cant_bytes_disponibles;
            cant_bytes_a_leer += cant_bytes_disponibles;
            for (int i = 0; i < cantidad_de_bloques_a_leer - 1; i++)
            {
                int offset_bloque_pi = punterosIndirectos[i] * pSuperbloque->block_size;
                if (pSuperbloque->block_size < cant_bytes)
                { // lees en todo el bloque y falta mas
                    memcpy(stream + cant_bytes_a_leer, copy_blocks + offset_bloque_pi, pSuperbloque->block_size);
                    log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                    //printf("en el puntero indirecto: %d \n", punterosIndirectos[i]); //PARA VER
                    //printf("LEO Contenido de copy_blocks bloque puntero indirecto: %s \n", stream + cant_bytes_a_leer); //PARA VER
                    cant_bytes = cant_bytes - pSuperbloque->block_size;
                    cant_bytes_a_leer = cant_bytes_a_leer + pSuperbloque->block_size;
                }
                else
                {

                    memcpy(stream + cant_bytes_a_leer, copy_blocks + offset_bloque_pi, cant_bytes);
                    log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                    //printf("en el puntero indirecto: %d \n", punterosIndirectos[i]); //PARA VER
                    //printf("LEO Contenido de copy_blocks bloque puntero indirecto: %s \n", stream + cant_bytes_a_leer); //PARA VER
                }
            }
        }
    }
    else
    {

        int offset_bloque_pArchivo_2 = offset_dentro_bloque_pArchivo; // agregooo

        numero_de_bloque_inicial -= 1; 
        for (int i = numero_de_bloque_inicial; i < numero_de_bloque_inicial + cantidad_de_bloques_a_leer; i++)
        {
            int offset_bloque_pi = punterosIndirectos[i] * pSuperbloque->block_size;
            if (pSuperbloque->block_size < cant_bytes)
            { // escribis en todo el bloque y falta mas
                memcpy(stream + cant_bytes_a_leer, copy_blocks + offset_bloque_pi + offset_dentro_bloque_pArchivo, pSuperbloque->block_size - offset_bloque_pArchivo_2);
                log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                offset_dentro_bloque_pArchivo = 0;
                //printf("LEO Contenido de copy_blocks bloque puntero indirecto: %s \n", stream+cant_bytes_a_leer); //PARA VER
                cant_bytes_a_leer = cant_bytes_a_leer + (pSuperbloque->block_size - offset_bloque_pArchivo_2);
                offset_bloque_pArchivo_2 = 0;
                cant_bytes = cant_bytes - cant_bytes_a_leer;
            }
            else
            {
                memcpy(stream + cant_bytes_a_leer, copy_blocks + offset_bloque_pi + offset_dentro_bloque_pArchivo, cant_bytes);
                log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                //printf("LEO Contenido de copy_blocks bloque puntero indirecto: %s \n", stream + cant_bytes_a_leer); //pARA VER
            }
        }
    }
    log_info(logger_main, "Leer archivo:  %s - Puntero: %d - Memoria: %d - Tamanio: %d", fcb->nombre_archivo, puntero_archivo, direccion_fisica, cant_bytes_inicial);


    t_info_write *info_write = malloc(sizeof(t_info_write));
    info_write->info = malloc(sizeof(t_info));
    info_write->base_address = direccion_fisica;
    info_write->info->data = malloc(cant_bytes);
    memcpy(info_write->info->data, stream, cant_bytes);
    info_write->info->size = cant_bytes;
    
    bool res = send_info_write(memory_socket, info_write, logger_aux);
    if(!res)
        log_error(logger_aux, "No se pudo enviar el info de F_READ a memoria");

    info_write_destroy(info_write);
    
    t_package *package = get_package(memory_socket, logger_aux);

    t_status_code status_code;
    if(package->operation_code == STATUS_CODE) {
        status_code = get_status_code(package);
        if(status_code == SUCCESS) {
            log_info(logger_aux, "Se pudo escribir en memoria en el F_READ");
        } else {
            log_error(logger_aux, "No se pudo escribir en memoria en el F_READ");
        }
    } else {
        log_error(logger_aux, "No se pudo obtener el OK de memoria en el F_READ");
    }

    package_destroy(package);


    fclose(file_fcb);
    free(fcb->nombre_archivo);
    free(fcb);
    free(ruta_Fcb);
    free(stream);
    free(punterosIndirectos);

    return 0;
}

int write_file(int puntero_archivo, char *nombre, int cant_bytes, int direccion_fisica)
{
    int cant_bytes_inicial = cant_bytes;
    char *ruta_Fcb = malloc(strlen(config->path_fcb) + strlen(nombre) + 2);
    strcpy(ruta_Fcb, config->path_fcb);
    strcat(ruta_Fcb, "/");
    strcat(ruta_Fcb, nombre);
    FILE *file_fcb = fopen(ruta_Fcb, "r");
    if (file_fcb == NULL)
        return 1;

    t_fcb *fcb = malloc(sizeof(t_fcb));
    fread(fcb, sizeof(t_fcb), 1, file_fcb);
    fcb->nombre_archivo=malloc(strlen(nombre)+1);
    strcpy(fcb->nombre_archivo, nombre);

    t_info_read* info_read = malloc(sizeof(t_info_read));
    info_read->base_address = direccion_fisica;
    info_read->size = cant_bytes;

    bool res = send_info_read(memory_socket, info_read, logger_aux);
    if(!res)
        log_error(logger_aux, "No se pudo enviar la request de lectura a memoria en el F_WRITE");
    free(info_read);

    t_package *package = get_package(memory_socket, logger_aux);

    t_info *info;
    if(package->operation_code == INFO) {
        info = get_info(package);
    } else {
        log_error(logger_aux, "No se pudo obtener el valor de memoria en la lectura de F_WRITE");
    }

    package_destroy(package);

    void *stream = malloc(cant_bytes);
    memcpy(stream, info->data, info->size);

    info_destroy(info);
    
    //void *stream= "meli aprobo sistemas operativos y ahora es una persona feliz ademas ya puede tener vacaciones y tirar cualquier cosa de la facu a la mierda si es que no tiene que rendir finales";
    
    int numero_de_bloque_inicial = puntero_archivo / pSuperbloque->block_size;
    int cantida_de_bloques_a_escribir = cant_bytes / pSuperbloque->block_size;
    int offset_aux = cant_bytes - cantida_de_bloques_a_escribir * pSuperbloque->block_size;                                        // desde donde empiezo a escribir en el bloque
    int offset_dentro_bloque_pArchivo = puntero_archivo - (puntero_archivo / pSuperbloque->block_size) * pSuperbloque->block_size; // desde donde empiezo a escribir en el bloque
    int cant_bytes_disponibles = pSuperbloque->block_size - offset_dentro_bloque_pArchivo;

    if (offset_aux > cant_bytes_disponibles)
    {
        cantida_de_bloques_a_escribir++;
    }

    if (cant_bytes % pSuperbloque->block_size > 0)
    {
        cantida_de_bloques_a_escribir++;
    }

    int *punterosIndirectos = NULL;
    int cant_bytes_escritos = 0;
    if (fcb->puntero_indirecto != 0)
    {
        punterosIndirectos = obtenerPunterosIndirectos(fcb);

        /*  PARA VER
        for(int i=0; i*sizeof(int)< pSuperbloque->block_size; i++){
            printf("que punteros indirectos hay aca: %d \n", punterosIndirectos[i]);
        }

        */
    }

    if (numero_de_bloque_inicial == 0)
    {
        if (cantida_de_bloques_a_escribir == 1)
        {
            memcpy(copy_blocks + fcb->puntero_directo * pSuperbloque->block_size + offset_dentro_bloque_pArchivo, stream, cant_bytes);
            log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, 0, fcb->puntero_directo);
            //printf("en el puntero directo: %d \n", fcb->puntero_directo); //PARA VER
        }
        else
        {
            memcpy(copy_blocks + fcb->puntero_directo * pSuperbloque->block_size + offset_dentro_bloque_pArchivo, stream, cant_bytes_disponibles);
            log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, 0, fcb->puntero_directo);

            //printf("en el puntero directo: %d \n", fcb->puntero_directo); //PARA VER             
            //printf("Contenido de copy_blocks bloque puntero directo: %s \n", (char*)copy_blocks + fcb->puntero_directo*pSuperbloque->block_size+offset_dentro_bloque_pArchivo); //PARA VER
            cant_bytes = cant_bytes - cant_bytes_disponibles;
            cant_bytes_escritos += cant_bytes_disponibles;
            for (int i = 0; i < cantida_de_bloques_a_escribir - 1; i++)
            {
                int offset_bloque_pi = punterosIndirectos[i] * pSuperbloque->block_size;
                if (pSuperbloque->block_size < cant_bytes)
                { // escribis en todo el bloque y falta mas
                    memcpy(copy_blocks + offset_bloque_pi, stream + cant_bytes_escritos, pSuperbloque->block_size);
                    log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                    //printf("en el puntero indirecto: %d \n", punterosIndirectos[i]); //PARA VER
                    //printf("Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi); //pARA VER

                    cant_bytes = cant_bytes - pSuperbloque->block_size;
                    cant_bytes_escritos = cant_bytes_escritos + pSuperbloque->block_size;
                }
                else
                {
                    memcpy(copy_blocks + offset_bloque_pi, stream + cant_bytes_escritos, cant_bytes);
                    log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                    //printf("Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi); //PARA VER
                }
            }
        }
    }
    else
    {

        int offset_bloque_pArchivo_2 = offset_dentro_bloque_pArchivo;

        numero_de_bloque_inicial -= 1; // original
        for (int i = numero_de_bloque_inicial; i < numero_de_bloque_inicial + cantida_de_bloques_a_escribir; i++)
        { // asi me aseguro q itere la cant de veces de cantbloquesaescribir
            //printf("puntero indirecto es: %d \n", punterosIndirectos[i]); //PARA VER
            int offset_bloque_pi = punterosIndirectos[i] * pSuperbloque->block_size;
            if (pSuperbloque->block_size < cant_bytes)
            { // escribis en todo el bloque y falta mas
                memcpy(copy_blocks + offset_bloque_pi + offset_dentro_bloque_pArchivo, stream + cant_bytes_escritos, pSuperbloque->block_size - offset_bloque_pArchivo_2);
                log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                cant_bytes_disponibles = 0;
                printf("Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi+offset_dentro_bloque_pArchivo); //pARA VER
                offset_dentro_bloque_pArchivo = 0;
                cant_bytes_escritos = cant_bytes_escritos + (pSuperbloque->block_size - offset_bloque_pArchivo_2);
                offset_bloque_pArchivo_2 = 0;
                cant_bytes = cant_bytes - cant_bytes_escritos;
            }
            else
            {

                memcpy(copy_blocks + offset_bloque_pi + offset_dentro_bloque_pArchivo, stream + cant_bytes_escritos, cant_bytes);
                log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                printf("Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi+offset_dentro_bloque_pArchivo); //PARA VER
            }
        }
    }

    log_info(logger_main, "Escribir archivo:  %s - Puntero: %d - Memoria: %d - Tamanio: %d", fcb->nombre_archivo, puntero_archivo, direccion_fisica, cant_bytes_inicial);

    fclose(file_fcb);
    free(fcb->nombre_archivo);
    free(fcb);
    free(ruta_Fcb);
    free(punterosIndirectos);
    //free(stream);

    return 0;
}
