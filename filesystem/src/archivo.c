#include "archivo.h"
//#include <fcntl.h> //para la funcion open 
//#include <sys/mman.h> //para el mmap
//#include <math.h> // para el ceil 


t_superbloque *pSuperbloque;
void* copy_blocks;
t_bitarray *bitmap;
int bitmap_size;


//void initialize_filesystem( t_config_filesystem *config ){
void initialize_filesystem( t_config_filesystem *config, t_log *logger_aux ){

create_fcb(config->path_fcb);

create_superbloque(config->path_superbloque);
create_bitmap(config);

pthread_t crear_y_manejar_blocks;
pthread_create(&crear_y_manejar_blocks,NULL,(void*)create_blocks,(void*)config);
pthread_detach(crear_y_manejar_blocks);


/*---------------PARA PROBAR SI FUNCIONA DESCOMENTAR ESTAS FUNCIONES DE ACA-----------*/

//Ejemplos crear archivos: 
create_file(config,"pepe",logger_aux); 
create_file(config,"juanma",logger_aux); 
create_file(config,"meli",logger_aux); 

//Ejemplos abrir archivos: 

//open_file(config,"pepe",logger_aux);
//open_file(config,"meli",logger_aux);


//Ejemplos truncar el mismo archivo con diferentes tamaños:  

truncate_file(config,200,"pepe", logger_aux);
truncate_file(config,300,"meli", logger_aux);
//truncate_file(config,150,"pepe",logger_aux);
//truncate_file(config,200,"pepe", logger_aux);

//Ejemplos escribir archivo: 

//write_file(config, 35, "pepe", 50, 3456789,logger_aux); 
write_file(config, 35, "pepe", 120, 3456789,logger_aux); 


//Ejemplos leer archivo: 

read_file(config, 35, "pepe", 120, 3456789,logger_aux); 
//read_file(config, 40, "pepe", 50, 3456789,logger_aux); 

/*-----------------------------------------------------------------------------------*/

}

void create_fcb(char *path_fcb){
    mkdir(path_fcb,0777);
}

void create_superbloque(char *path_superblock){

    pSuperbloque= malloc(sizeof(t_superbloque));
    //hardcodeado
    pSuperbloque ->block_size = 64;
    pSuperbloque ->block_count = 20;

    int file_superblock = open(path_superblock, O_CREAT | O_RDWR, 0644);

    write(file_superblock, &(pSuperbloque->block_size), sizeof(uint32_t));
    write(file_superblock, &(pSuperbloque->block_count), sizeof(uint32_t));
    //fwrite(&(pSuperbloque->block_size), sizeof(uint32_t), 1, file_superblock);
    
    close(file_superblock);
   // free(pSuperbloque); 
}

   /*
      block_size es de 4 bytes 
      blocks es de 4 bytes 
      => tamaño del superbloque es 8 bytes (2*sizeof(uint32_t))



*/ 

void create_bitmap(t_config_filesystem *config ){

   // int bitmap_size= ceil(pSuperbloque->block_count / 8 );   //NO PONER 8.0 ROMPE EL CEIL 
    bitmap_size= ((pSuperbloque->block_count + 7) / 8);
    
    int file_bitmap = open(config->path_bitmap, O_CREAT | O_RDWR, 0644);

    ftruncate(file_bitmap,sizeof(t_superbloque) + bitmap_size);

    void *mmapBitmap = mmap(NULL, sizeof(t_superbloque) + bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_bitmap, 0);

    bitmap = bitarray_create_with_mode(mmapBitmap + sizeof(t_superbloque), bitmap_size, LSB_FIRST);
    //bitarray_set_bit(bitmap, false);//pone en 0 c/ bit 
    for (int i = 0; i < pSuperbloque->block_count; i++) {
    bitarray_clean_bit(bitmap, i);
    }

    test_bitmap_zeros(bitmap);

    //bitarray_destroy(bitmap);
    
    msync(mmapBitmap, sizeof(t_superbloque) + bitmap_size, MS_SYNC);

    munmap(mmapBitmap, sizeof(t_superbloque) + bitmap_size); //libero memoria mapeada de mmap
    
    //free(pSuperbloque);
    close(file_bitmap);

}

void test_bitmap_zeros(t_bitarray *bitmap) {
    int bitmap_size = bitarray_get_max_bit(bitmap);

    for (int i = 0; i < bitmap_size; i++) {
        if (bitarray_test_bit(bitmap, i)) {
            printf("El bit en la posición %d no está en 0\n", i);
            return;
        }
    }

   // printf("Todos los bits del bitmap están en 0\n");
}



void create_blocks(t_config_filesystem *config){

    //t_superbloque *pSuperbloque= malloc(sizeof(t_superbloque));

    int file_blocks= open(config->path_bloques,O_CREAT | O_RDWR, 0644);

    int file_block_size = pSuperbloque->block_size * pSuperbloque->block_count;

    ftruncate(file_blocks,file_block_size );

    void* mmapBlocks = mmap(NULL, file_block_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_blocks, 0);

    copy_blocks = malloc(file_block_size);
    //semaforos
    memcpy(copy_blocks,mmapBlocks,file_block_size);

    while(1){
    // Sincronizas los cambios en el archivo de bloques en disco
        sleep(config->retardo_acceso_bloque);
        memcpy(mmapBlocks,copy_blocks,file_block_size);
        sync_blocks(mmapBlocks, file_block_size);
    }
    
    //free(copy_blocks);
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

//como me dice q devuelve siempre OK entonces es una funcion q devuelve algo, es decir no puede ser void 

int create_file(t_config_filesystem *config ,char *nombre, t_log *logger_aux){

    char* ruta_Fcb=malloc(strlen(config->path_fcb)+strlen(nombre)+2);
    strcpy(ruta_Fcb,config->path_fcb);
    strcat(ruta_Fcb,"/");
    strcat(ruta_Fcb,nombre);
    FILE* file_fcb = fopen(ruta_Fcb,"w");
    t_fcb*fcb = malloc(sizeof(t_fcb));
    fcb->nombre_archivo=malloc(strlen(nombre)+1);

    strcpy(fcb->nombre_archivo, nombre);
    fcb->tamanio_archivo = 0;
    fcb->puntero_directo = 0;
    fcb->puntero_indirecto = 0;

    fwrite(fcb, sizeof(t_fcb), 1, file_fcb);
    fclose(file_fcb);
    log_info(logger_aux,"Crear archivo: %s", fcb->nombre_archivo);
    
    free(ruta_Fcb);
    //free(fcb->nombre_archivo);
    free(fcb); 
    return 0; //devuelvo 0 siempre porque es exitoso, siempre se puede crear el archivo
    //return OK   podria llegar a implementar el OK en vez del 0 , preguntar!!!
}


int obtener_bloque_libre(t_bitarray *bitmap, int bitmap_size){

//obtengo el bloque libre y luego lo asigno como ocupado en el bitmap
    int bloque_libre;

    //bitmap_size= bitarray_get_max_bit(bitmap);
    for (int i = 0; i < pSuperbloque->block_count; i++) {
        if(!bitarray_test_bit(bitmap,i)){ //el bit en la pos i del bitmap es 0 osea esta libre => 
            bitarray_set_bit(bitmap, i); //lo seteo como ocupado (1)
            bloque_libre = i;
            printf("bloque ahora asignado como ocupado en el bitmap es: %d\n", bloque_libre);
            break;
        }
         //printf("bloque ocupado en el bitmap es: %d\n", bloque_libre);

    }
    return bloque_libre;
}


int open_file(t_config_filesystem *config ,char *nombre, t_log *logger_aux){

    char* ruta_Fcb=malloc(strlen(config->path_fcb)+strlen(nombre)+2);
    strcpy(ruta_Fcb,config->path_fcb);
    strcat(ruta_Fcb,"/");
    strcat(ruta_Fcb,nombre);
    FILE* file_fcb = fopen(ruta_Fcb,"r");

    t_fcb*fcb = malloc(sizeof(t_fcb));
    fcb->nombre_archivo=malloc(strlen(nombre)+1);

    strcpy(fcb->nombre_archivo, nombre);

    
    if(file_fcb!=NULL){

        fread(fcb, sizeof(t_fcb), 1, file_fcb); // Leer los datos del archivo y almacenarlos en fcb
        log_info(logger_aux,"Abrir archivo:  %s", fcb->nombre_archivo);
        fclose(file_fcb);
        //log_info(logger_aux,"Abrir archivo:  %s", fcb->nombre_archivo);

        return 0; //return OK 

    }else{
        printf("No existe el archivo: %s \n", file_fcb );
    }
}

int truncate_file(t_config_filesystem *config, int nuevo_tamanio, char *nombre, t_log *logger_aux){

    char* ruta_Fcb=malloc(strlen(config->path_fcb)+strlen(nombre)+2);
    strcpy(ruta_Fcb,config->path_fcb);
    strcat(ruta_Fcb,"/");
    strcat(ruta_Fcb,nombre);
    FILE* file_fcb = fopen(ruta_Fcb,"r");
    if(file_fcb!=NULL){
    t_fcb*fcb = malloc(sizeof(t_fcb));
    fcb->nombre_archivo=malloc(strlen(nombre)+1);  //  a ver si agregando esto se soluciona
	fread(fcb, sizeof(t_fcb), 1, file_fcb);

    int *punterosIndirectos;
    if(fcb->puntero_indirecto!=0){
        punterosIndirectos=obtenerPunterosIndirectos(fcb,logger_aux);
    }
    if(nuevo_tamanio > fcb->tamanio_archivo){ //CASO EXPANDIR TAMAÑO
        int bloques_Adicionales= calculate_additionalBlocks(nuevo_tamanio,fcb);
        printf("Se requieren %d bloques adicionales \n", bloques_Adicionales);
    
        if(bloques_Adicionales > 0){
            //punterosIndirectos=malloc((bloques_Adicionales-1)*sizeof(int));
            int i; 
            int bloque_libre;
            //entonces hay un puntero indirecto
            int bloque_libre_puntero_indirecto=0;
            if(fcb->puntero_indirecto==0){
                bloque_libre= obtener_bloque_libre(bitmap,bitmap_size);//bloque directo
                fcb->puntero_directo=bloque_libre;
                 if(bloques_Adicionales>1){
                 bloque_libre_puntero_indirecto=obtener_bloque_libre(bitmap,bitmap_size);
                fcb->puntero_indirecto=bloque_libre_puntero_indirecto;
                punterosIndirectos=malloc((bloques_Adicionales-1)*sizeof(int));
                for(i=0 ; i < bloques_Adicionales-1; i++ ){
                    
                   bloque_libre= obtener_bloque_libre(bitmap,bitmap_size);
                   punterosIndirectos[i]=bloque_libre;//asigno el bloque libre a los punteros q faltan para expandir el tamaño
                }
                }
            }else{
            bloque_libre_puntero_indirecto=fcb->puntero_indirecto;
            int z=0;
            for(i=0 ; i < bloques_Adicionales; i++ ){
                   bloque_libre= obtener_bloque_libre(bitmap,bitmap_size);
                   
                   while (punterosIndirectos[z]!=0 && punterosIndirectos[z]!=9999)
                   {
                    z++;
                   }
                   punterosIndirectos[z]=bloque_libre;//asigno el bloque libre a los punteros q faltan para expandir el tamaño
                }
            }
                int cantPunterosIndirectos=cantidadPunterosIndirectos(punterosIndirectos);
                memcpy(copy_blocks + bloque_libre_puntero_indirecto*pSuperbloque->block_size,punterosIndirectos,sizeof(int)*cantPunterosIndirectos);
                
        }

        for (int i = 0; i < bitmap_size * 8 ; i++) {
        int bit_value = bitarray_test_bit(bitmap, i);
        printf("Bloque: %d estado: %d\n", i, bit_value);
        }

        fcb->tamanio_archivo= nuevo_tamanio; //cambio el tamanio al nuevo 
        fclose(file_fcb);
        FILE* file_fcb_escritura = fopen(ruta_Fcb,"w");
        fwrite(fcb, sizeof(t_fcb), 1, file_fcb); //escribo en el fcb el cambio de tamaño 
        fclose(file_fcb_escritura);
        log_info(logger_aux,"Truncar archivo:  %s - Tamaño: %d ", fcb->nombre_archivo, fcb-> tamanio_archivo);
        free(ruta_Fcb);
        free(fcb);

    } else if (nuevo_tamanio < fcb->tamanio_archivo) { //CASO ACHICHAR TAMAÑO 

        int bloques_a_liberar= calculate_freeBlocks(nuevo_tamanio,fcb);

        printf("Se requieren %d bloques a liberar \n", bloques_a_liberar);
        
        int cantidad_bloques_tam_original= cantidadPunterosIndirectos(punterosIndirectos)+1; 
        
        
        if (bloques_a_liberar > 0 && nuevo_tamanio!=0) {
            int i=0; 
            int bloque_a_liberar;
            int cantPunterosIndirectos= cantidadPunterosIndirectos(punterosIndirectos);
                if(cantPunterosIndirectos==bloques_a_liberar){
                    int bloque_a_eliminar_pi=fcb->puntero_indirecto;
                    set_freeBlock_bitmap(bitmap,bloque_a_eliminar_pi);
                }

                for (int i = bloques_a_liberar ; i > 0; i--){ //voy decrementando los bloques q libero
                    bloque_a_liberar= punterosIndirectos[cantPunterosIndirectos-i]; 
                    set_freeBlock_bitmap(bitmap, bloque_a_liberar);
                    punterosIndirectos[cantPunterosIndirectos-i] = 0; // Asigno valor inválido al puntero liberado
                } 
                if(cantPunterosIndirectos>bloques_a_liberar){
                    memcpy(copy_blocks + fcb->puntero_indirecto*pSuperbloque->block_size,punterosIndirectos,sizeof(int)*cantPunterosIndirectos);
                } 

        }else if(nuevo_tamanio == 0){
            if(fcb->puntero_indirecto != 0){
                for(int i=0; i<bloques_a_liberar; i++){
                    int bloque_a_eliminar= punterosIndirectos[i]; 
                    set_freeBlock_bitmap(bitmap,bloque_a_eliminar); 
                }
                int bloque_a_eliminar_pi= fcb->puntero_indirecto; 
                set_freeBlock_bitmap(bitmap,bloque_a_eliminar_pi);
            }        
            
            int bloque_a_eliminar_pd= fcb->puntero_directo; 
            set_freeBlock_bitmap(bitmap,bloque_a_eliminar_pd); 
        }
        fcb->tamanio_archivo= nuevo_tamanio;
        fclose(file_fcb);
        FILE* file_fcb_escritura = fopen(ruta_Fcb,"w");
        fwrite(fcb, sizeof(t_fcb), 1, file_fcb_escritura);
        fclose(file_fcb_escritura);
        log_info(logger_aux,"Truncar archivo:  %s - Tamaño: %d ", fcb->nombre_archivo, fcb-> tamanio_archivo);
        free(fcb);
        free(ruta_Fcb);
    }   
    return 0;//OK
    }
    return 1;//FAIL
}

//---------------------funciones aux para expandir el tamaño----------------------------------------//

int calculate_additionalBlocks(int nuevo_tamanio, t_fcb *fcb){

    //int cant_bloques_actual= ceil(fcb->tamanio_archivo / pSuperbloque->block_size);
    int cant_bloques_actual = fcb->tamanio_archivo / pSuperbloque->block_size;
    if (fcb->tamanio_archivo % pSuperbloque->block_size != 0) {
        cant_bloques_actual += 1;
    }    //int cant_bloques_nuevo=(double)ceil(nuevo_tamanio/pSuperbloque->block_size);

    int cant_bloques_nuevo = nuevo_tamanio / pSuperbloque->block_size;
    if (nuevo_tamanio % pSuperbloque->block_size != 0) {
        cant_bloques_nuevo += 1;
    }

    int bloques_adicionales= cant_bloques_nuevo-cant_bloques_actual;
    return bloques_adicionales;
}

int get_freeBlock(t_bitarray *bitmap, int bitmap_size) {
    int bloque_libre = -1;

    bitmap_size= bitarray_get_max_bit(bitmap);
    for (int i = 0; i < pSuperbloque->block_count; i++) {

        if(!bitarray_test_bit(bitmap,i)){ //el bit en la pos i del bitmap es 0 osea esta libre => 
            bitarray_set_bit(bitmap, i); //lo seteo como ocupado (1)
            //CREAR LOG QUE INDIQUE QUE EL BLOQUE i ESTA LIBRE
            bloque_libre = i;
        }
        else{
            //CREAR LOG QUE INDIQUE QUE EL BLOQUE i ESTA OCUPADO
        }
    }
    return bloque_libre;
}


//-------------------------funciones aux para reducir tamaño ----------------------------------//

int calculate_freeBlocks(int nuevo_tamanio, t_fcb *fcb){
//int bloques_actuales = ceil((double)fcb->tamanio_archivo/ pSuperbloque->block_size);
//int bloques_nuevos = ceil((double)nuevo_tamanio / pSuperbloque->block_size);

    //mientras tanto se calcula de esta manera
    int bloques_actuales = fcb->tamanio_archivo / pSuperbloque->block_size;
    if (fcb->tamanio_archivo % pSuperbloque->block_size > 0) {
        bloques_actuales++;
    }

    int bloques_nuevos = nuevo_tamanio/ pSuperbloque->block_size;
    if (fcb->tamanio_archivo % pSuperbloque->block_size > 0) {
        bloques_nuevos++;
    }

    int bloques_libres= bloques_actuales - bloques_nuevos;
    return bloques_libres;

}

void set_freeBlock_bitmap(t_bitarray *bitmap, int bloque){
    bitarray_clean_bit(bitmap, bloque);
}


int* obtenerPunterosIndirectos(t_fcb* fcb,t_log* logger_aux){
    int* punterosIndirectosActuales=malloc(pSuperbloque->block_size);
    int j=0;
    while (j*sizeof(int)<=pSuperbloque->block_size){
        punterosIndirectosActuales[j]=9999;
        j++;
    }    
    if (fcb->puntero_indirecto!=0){
        //for(int i=0;i<pSuperbloque->block_size/sizeof(int);i++){
            int z=0;
            memcpy(&punterosIndirectosActuales[z],copy_blocks+pSuperbloque->block_size+z*sizeof(int),sizeof(int));
            log_info(logger_aux," puntero indirecto: %d",punterosIndirectosActuales[z]);
            z++;
            while(punterosIndirectosActuales[z-1]!=0 && (z)*sizeof(int)<pSuperbloque->block_size){
            memcpy(&punterosIndirectosActuales[z],copy_blocks+pSuperbloque->block_size+z*sizeof(int),sizeof(int));
            log_info(logger_aux,"puntero indirecto %d",punterosIndirectosActuales[z]);
            z++;
            }
        //}
    return punterosIndirectosActuales;
    }
    return NULL;

}
int cantidadPunterosIndirectos(int* punterosIndirectos){
    int z=0;
    while (punterosIndirectos[z]!=0 &&punterosIndirectos[z]!=9999)
    {
        z++;
    }
    return z;
    
}
/**.....................................................................................................*/

int read_file(t_config_filesystem *config, int puntero_archivo, char *nombre, int cant_bytes, void *direccion_fisica, t_log *logger_aux){

    int cant_bytes_inicial=cant_bytes;
    char* ruta_Fcb=malloc(strlen(config->path_fcb)+strlen(nombre)+2);
    strcpy(ruta_Fcb,config->path_fcb);
    strcat(ruta_Fcb,"/");
    strcat(ruta_Fcb,nombre);
    FILE* file_fcb = fopen(ruta_Fcb,"r");
    if(file_fcb==NULL){
        return 1;
    }

    t_fcb*fcb = malloc(sizeof(t_fcb));
    fcb->nombre_archivo=malloc(strlen(nombre)+1);
    fread(fcb, sizeof(t_fcb), 1, file_fcb);

    void *buffer = malloc(cant_bytes);
    

    int numero_de_bloque_inicial= puntero_archivo/pSuperbloque->block_size;
    int cantidad_de_bloques_a_leer=cant_bytes/pSuperbloque->block_size;
    int offset_aux= cant_bytes-cantidad_de_bloques_a_leer*pSuperbloque->block_size; //desde donde empiezo a escribir en el bloque
    int offset_dentro_bloque_pArchivo= puntero_archivo-(puntero_archivo/pSuperbloque->block_size)*pSuperbloque->block_size; //desde donde empiezo a escribir en el bloque
    int cant_bytes_disponibles= pSuperbloque-> block_size - offset_dentro_bloque_pArchivo;

    if(offset_aux>cant_bytes_disponibles){
        cantidad_de_bloques_a_leer++;
    }

    if (cant_bytes % pSuperbloque->block_size > 0) {
        cantidad_de_bloques_a_leer++;
    }

    int *punterosIndirectos;
    int cant_bytes_a_leer=0;
    
    if(fcb->puntero_indirecto!=0){
    punterosIndirectos=obtenerPunterosIndirectos(fcb,logger_aux);
    }

    if(numero_de_bloque_inicial==0){
        if(cantidad_de_bloques_a_leer==1){
            memcpy(buffer, copy_blocks +fcb->puntero_directo*pSuperbloque->block_size+ offset_dentro_bloque_pArchivo,cant_bytes); 
            //memcpy(copy_blocks +fcb->puntero_directo*pSuperbloque->block_size+ offset_dentro_bloque_pArchivo, stream,cant_bytes); 
        }
        else{
            memcpy(buffer,copy_blocks +fcb->puntero_directo*pSuperbloque->block_size+ offset_dentro_bloque_pArchivo,cant_bytes_disponibles);
            //memcpy(copy_blocks +fcb->puntero_directo*pSuperbloque->block_size+ offset_dentro_bloque_pArchivo, stream,cant_bytes_disponibles);
            
            printf("en el puntero directo: %d \n", fcb->puntero_directo);
            printf("LEO Contenido de copy_blocks bloque puntero directo: %s \n", (char*)copy_blocks + offset_dentro_bloque_pArchivo);

            cant_bytes=cant_bytes-cant_bytes_disponibles; 
            cant_bytes_a_leer+=cant_bytes_disponibles;
            for(int i=0; i < cantidad_de_bloques_a_leer-1; i++){
                int offset_bloque_pi= punterosIndirectos[i] * pSuperbloque-> block_size;        
                if(pSuperbloque->block_size <  cant_bytes){//lees en todo el bloque y falta mas
                    memcpy(buffer + cant_bytes_a_leer,copy_blocks + offset_bloque_pi, pSuperbloque->block_size);
                    //memcpy(copy_blocks + offset_bloque_pi, stream + cant_bytes_escritos, pSuperbloque->block_size);
                    printf("en el puntero indirecto: %d \n", punterosIndirectos[i]);
                    printf("LEO Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi);
                    cant_bytes= cant_bytes -pSuperbloque->block_size; 
                    cant_bytes_a_leer= cant_bytes_a_leer + pSuperbloque->block_size; 
                }
                else{
                    
                    memcpy(buffer + cant_bytes_a_leer, copy_blocks + offset_bloque_pi,cant_bytes);
                    //memcpy(copy_blocks + offset_bloque_pi, stream + cant_bytes_escritos,cant_bytes);
                    printf("LEO Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi);

                }
            }
        }
    }
    else{
        numero_de_bloque_inicial-=1;
        for(int i=numero_de_bloque_inicial; i < cantidad_de_bloques_a_leer; i++){
            int offset_bloque_pi= punterosIndirectos[i] * pSuperbloque-> block_size;        
            if(pSuperbloque->block_size <  cant_bytes){//escribis en todo el bloque y falta mas

                memcpy(buffer + cant_bytes_a_leer, copy_blocks + offset_bloque_pi+offset_dentro_bloque_pArchivo, pSuperbloque->block_size);
                //memcpy(copy_blocks + offset_bloque_pi+offset_dentro_bloque_pArchivo, stream + cant_bytes_escritos, pSuperbloque->block_size);
                offset_dentro_bloque_pArchivo=0;
                printf("LEO Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi);
                cant_bytes_a_leer= cant_bytes_a_leer + pSuperbloque->block_size; 
                cant_bytes= cant_bytes -cant_bytes_a_leer; 
            }
            else{
                memcpy(buffer + cant_bytes_a_leer, copy_blocks + offset_bloque_pi+offset_dentro_bloque_pArchivo,cant_bytes);
                //memcpy(copy_blocks + offset_bloque_pi+offset_dentro_bloque_pArchivo, stream + cant_bytes_escritos,cant_bytes);
            }
        }
    }
    log_info(logger_aux,"Leer archivo:  %s - Puntero: %d - Memoria: %d - Tamanio: %d", fcb->nombre_archivo, puntero_archivo, direccion_fisica, cant_bytes_inicial);
    return 0;
}


 //send_info_read(socket_memoria, stream, log); // ENVIA A MEMORIA A ESCRIBIR
 //t_package *package = get_package(socket, logger);
//t_status_code sc = get_status_code(package); // ESPERA RESPUESTA DE MEMORIA
    //if(sc == FILE_READ)
    //    send_status_code(socket_kernel, FILE_READ, log); // AVISA A KERNEL QUE LEYO BIEN




int write_file(t_config_filesystem *config, int puntero_archivo, char *nombre, int cant_bytes, void *direccion_fisica, t_log *logger_aux){
    int cant_bytes_inicial=cant_bytes;
    char* ruta_Fcb=malloc(strlen(config->path_fcb)+strlen(nombre)+2);
    strcpy(ruta_Fcb,config->path_fcb);
    strcat(ruta_Fcb,"/");
    strcat(ruta_Fcb,nombre);
    FILE* file_fcb = fopen(ruta_Fcb,"r");
    if(file_fcb==NULL){
        return 1;
    }
        t_fcb*fcb = malloc(sizeof(t_fcb));
        fcb->nombre_archivo=malloc(strlen(nombre)+1);
        fread(fcb, sizeof(t_fcb), 1, file_fcb);
        //solicito a memoria la info q tengo q escribir 
        //request_info = (DIRMEMORIA + BYTES)
        //send_request_info(socket_memoria, request_info, log);    
        //recibo en un paquete el stream desde memoria lo q tengo q escribir (implementar)
        // t_package *package = get_package(socket, logger);
        //void *stream = get_info_write(package); // LEE MEMORIA
        void *stream = malloc(cant_bytes);
        stream= "meliaprobosistemasoperativosyahoraesunapersonafeliz holaquetal quieroversiescribeenotrosbloques a ver que onda si se escribe esto en los punteros indirectos";

        int numero_de_bloque_inicial= puntero_archivo/pSuperbloque->block_size;
        int cantida_de_bloques_a_escribir=cant_bytes/pSuperbloque->block_size;
        int offset_aux= cant_bytes-cantida_de_bloques_a_escribir*pSuperbloque->block_size; //desde donde empiezo a escribir en el bloque
        int offset_dentro_bloque_pArchivo= puntero_archivo-(puntero_archivo/pSuperbloque->block_size)*pSuperbloque->block_size; //desde donde empiezo a escribir en el bloque
        int cant_bytes_disponibles= pSuperbloque-> block_size - offset_dentro_bloque_pArchivo;

        if(offset_aux>cant_bytes_disponibles){
            cantida_de_bloques_a_escribir++;
        }
        if (cant_bytes % pSuperbloque->block_size > 0) {
            cantida_de_bloques_a_escribir++;
        }

    int *punterosIndirectos;
    int cant_bytes_escritos=0;
    if(fcb->puntero_indirecto!=0){
    punterosIndirectos=obtenerPunterosIndirectos(fcb,logger_aux);
    }
    if(numero_de_bloque_inicial==0){
        if(cantida_de_bloques_a_escribir==1){
            memcpy(copy_blocks +fcb->puntero_directo*pSuperbloque->block_size+ offset_dentro_bloque_pArchivo, stream,cant_bytes); 
        }
        else{
            memcpy(copy_blocks +fcb->puntero_directo*pSuperbloque->block_size+ offset_dentro_bloque_pArchivo, stream,cant_bytes_disponibles);
            
            printf("en el puntero directo: %d \n", fcb->puntero_directo);
            printf("Contenido de copy_blocks bloque puntero directo: %s \n", (char*)copy_blocks + offset_dentro_bloque_pArchivo);

            cant_bytes=cant_bytes-cant_bytes_disponibles; 
            cant_bytes_escritos+=cant_bytes_disponibles;
            for(int i=0; i < cantida_de_bloques_a_escribir-1; i++){
                int offset_bloque_pi= punterosIndirectos[i] * pSuperbloque-> block_size;        
                if(pSuperbloque->block_size <  cant_bytes){//escribis en todo el bloque y falta mas
                    memcpy(copy_blocks + offset_bloque_pi, stream + cant_bytes_escritos, pSuperbloque->block_size);
                    printf("en el puntero indirecto: %d \n", punterosIndirectos[i]);
                    printf("Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi);
                    cant_bytes= cant_bytes -pSuperbloque->block_size; 
                    cant_bytes_escritos= cant_bytes_escritos + pSuperbloque->block_size; 
                }
                else{
                    memcpy(copy_blocks + offset_bloque_pi, stream + cant_bytes_escritos,cant_bytes);
                    
                    printf("Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi);

                }
            }
        }
    }
    else{
        numero_de_bloque_inicial-=1;
        for(int i=numero_de_bloque_inicial; i < cantida_de_bloques_a_escribir; i++){
            int offset_bloque_pi= punterosIndirectos[i] * pSuperbloque-> block_size;        
            if(pSuperbloque->block_size <  cant_bytes){//escribis en todo el bloque y falta mas
                memcpy(copy_blocks + offset_bloque_pi+offset_dentro_bloque_pArchivo, stream + cant_bytes_escritos, pSuperbloque->block_size);
                offset_dentro_bloque_pArchivo=0;
                printf("Contenido de copy_blocks bloque puntero indirecto: %s \n", (char*)copy_blocks + offset_bloque_pi);
                cant_bytes_escritos= cant_bytes_escritos + pSuperbloque->block_size; 
                cant_bytes= cant_bytes -cant_bytes_escritos; 
            }
            else{
                memcpy(copy_blocks + offset_bloque_pi+offset_dentro_bloque_pArchivo, stream + cant_bytes_escritos,cant_bytes);
            }
        }
    }
    log_info(logger_aux,"Escribir archivo:  %s - Puntero: %d - Memoria: %d - Tamanio: %d", fcb->nombre_archivo, puntero_archivo, direccion_fisica, cant_bytes_inicial);
    return 0;
}
