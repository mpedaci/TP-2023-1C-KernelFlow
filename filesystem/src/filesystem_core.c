#include "filesystem_core.h"

void end_filesystem()
{
    // SINCRONIZO LOS ULTIMOS CAMBIOS
    if (bitmap_file != -1)
    {
        save_bitmap();
        munmap(bitmap->bitarray, get_bitmap_size());
        close(bitmap_file);
    }
    if (blocks_file != -1)
    {
        msync(blocks, get_blocks_size(), MS_SYNC);
        munmap(blocks, get_blocks_size());
        close(blocks_file);
    }
    if (bitmap != NULL)
        free(bitmap);
    if (superbloque != NULL)
        free(superbloque);
}

bool initialize_filesystem()
{
    create_fcb_dir();
    if (!load_superbloque())
    {
        log_error(logger_main, "No se pudo cargar el superbloque");
        return false;
    }
    if (!load_bitmap())
    {
        log_error(logger_main, "No se pudo cargar el bitmap de bloques");
        return false;
    }
    if (!load_blocks())
    {
        log_error(logger_main, "No se pudo cargar el archivo de bloques");
        return false;
    }
    return true;
}

void create_fcb_dir()
{
    mkdir(config_fs->path_fcb, 0777);
}

bool load_superbloque()
{
    superbloque = malloc(sizeof(t_superbloque));
    t_config *config = config_create(config_fs->path_superbloque);
    if (config == NULL)
        return false;
    superbloque->block_size = config_get_int_value(config, "BLOCK_SIZE");
    superbloque->block_count = config_get_int_value(config, "BLOCK_COUNT");
    config_destroy(config);
    return true;
}

t_fcb *load_fcb(char *filename)
{
    char *fcb_path = get_fcb_path(filename);
    t_config *config = config_create(fcb_path);
    if (config == NULL)
    {
        free(fcb_path);
        return NULL;
    }
    t_fcb *fcb = malloc(sizeof(t_fcb));
    fcb->nombre_archivo = string_duplicate(config_get_string_value(config, "NOMBRE_ARCHIVO"));
    fcb->tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");
    fcb->puntero_directo = config_get_int_value(config, "PUNTERO_DIRECTO");
    fcb->puntero_indirecto = config_get_int_value(config, "PUNTERO_INDIRECTO");
    config_destroy(config);
    free(fcb_path);
    return fcb;
}

void update_fcb(t_fcb *fcb)
{
    char *fcb_path = get_fcb_path(fcb->nombre_archivo);
    t_config *config = config_create(fcb_path);
    char tamanio_archivo[16];
    char puntero_directo[16];
    char puntero_indirecto[16];
    sprintf(tamanio_archivo,"%u", fcb->tamanio_archivo);
    sprintf(puntero_directo,"%u", fcb->puntero_directo);
    sprintf(puntero_indirecto,"%u", fcb->puntero_indirecto);
    config_set_value(config, "TAMANIO_ARCHIVO", tamanio_archivo);
    config_set_value(config, "PUNTERO_DIRECTO", puntero_directo);
    config_set_value(config, "PUNTERO_INDIRECTO", puntero_indirecto);
    config_save(config);
    config_destroy(config);
    free(fcb_path);
}

bool create_fcb(char *filename)
{
    char *fcb_path = get_fcb_path(filename);
    char *zero = "0";
    t_config *config = malloc(sizeof(t_config));
    config->path = string_duplicate(fcb_path);
    config->properties = dictionary_create();
    dictionary_put(config->properties, "NOMBRE_ARCHIVO", string_duplicate(filename));
    dictionary_put(config->properties, "TAMANIO_ARCHIVO", string_duplicate(zero));
    dictionary_put(config->properties, "PUNTERO_DIRECTO", string_duplicate(zero));
    dictionary_put(config->properties, "PUNTERO_INDIRECTO", string_duplicate(zero));
    config_save(config);
    config_destroy(config);
    free(fcb_path);
    return true;
}

char *get_fcb_path(char *filename)
{
    char *filepath = malloc(strlen(config_fs->path_fcb) + strlen(filename) + 2);
    strcpy(filepath, config_fs->path_fcb);
    strcat(filepath, "/");
    strcat(filepath, filename);
    return filepath;
}

int get_bitmap_size()
{
    return ((superbloque->block_count + 7) / 8);
}

void save_bitmap()
{
    msync(bitmap->bitarray, get_bitmap_size(), MS_SYNC);
}

bool load_bitmap()
{
    bitmap_file = open(config_fs->path_bitmap, O_CREAT | O_RDWR, 0644);
    struct stat s;
    fstat(bitmap_file, &s);
    size_t size = s.st_size;
    void *mmapBitmap;
    if (size <= 0) // El archivo no esta creado se crea y se inicializa en 0
    {
        ftruncate(bitmap_file, get_bitmap_size());
        mmapBitmap = mmap(NULL, get_bitmap_size(), PROT_READ | PROT_WRITE, MAP_SHARED, bitmap_file, 0);
        bitmap = bitarray_create_with_mode(mmapBitmap, get_bitmap_size(), LSB_FIRST);
        for (int i = 0; i < superbloque->block_count; i++)
            bitarray_clean_bit(bitmap, i);
        save_bitmap();
    }
    else // El archivo ya esta creado se mapea
    {
        mmapBitmap = mmap(NULL, get_bitmap_size(), PROT_READ | PROT_WRITE, MAP_SHARED, bitmap_file, 0);
        bitmap = bitarray_create_with_mode(mmapBitmap, get_bitmap_size(), LSB_FIRST);
    }
    return true;
}

int get_blocks_size()
{
    return superbloque->block_size * superbloque->block_count;
}

void save_blocks()
{
    msync(blocks, get_blocks_size(), MS_SYNC);
}

bool load_blocks()
{
    blocks_file = open(config_fs->path_bloques, O_CREAT | O_RDWR, 0644);
    struct stat s;
    fstat(blocks_file, &s);
    size_t size = s.st_size;
    if (size <= 0) // El archivo no esta creado se crea y se inicializa en 0
    {
        ftruncate(blocks_file, get_blocks_size());
        blocks = mmap(NULL, get_blocks_size(), PROT_READ | PROT_WRITE, MAP_SHARED, blocks_file, 0);
        msync(blocks, get_blocks_size(), MS_SYNC);
    }
    else // El archivo ya esta creado se mapea
    {
        blocks = mmap(NULL, get_blocks_size(), PROT_READ | PROT_WRITE, MAP_SHARED, blocks_file, 0);
    }
    return true;
}

uint32_t get_cpb()
{
    return superbloque->block_size / sizeof(uint32_t);
}