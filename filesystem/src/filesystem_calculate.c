#include "filesystem_calculate.h"

uint32_t calculate_additionalBlocks(t_fcb *fcb, uint32_t nuevo_tamanio)
{
    uint32_t cant_bloques_actual = (fcb->tamanio_archivo + superbloque->block_size - 1) / superbloque->block_size;
    uint32_t cant_bloques_nuevo = (nuevo_tamanio + superbloque->block_size - 1) / superbloque->block_size;
    uint32_t bloques_adicionales = cant_bloques_nuevo - cant_bloques_actual;
    return bloques_adicionales;
}

int obtener_bloque_libre()
{
    for (int i = 0; i < superbloque->block_count; i++)
    {
        if (!bitarray_test_bit(bitmap, i))
        {
            bitarray_set_bit(bitmap, i);
            log_info(logger_main, "Acceso a Bitmap - Bloque: %d - Estado: 0 LIBRE", i);
            return i;
        }
        else
            log_info(logger_main, "Acceso a Bitmap - Bloque: %d - Estado: 1 OCUPADO", i);
    }
    return -1;
}

//-------------------------funciones aux para reducir tamaño ----------------------------------//

uint32_t calculate_freeBlocks(t_fcb *fcb, uint32_t nuevo_tamanio)
{
    uint32_t bloques_actuales = (fcb->tamanio_archivo + superbloque->block_size - 1) / superbloque->block_size;
    uint32_t bloques_nuevos = (nuevo_tamanio + superbloque->block_size - 1) / superbloque->block_size;
    uint32_t bloques_libres = bloques_actuales - bloques_nuevos;
    return bloques_libres;
}

void set_freeBlock_bitmap(int bloque)
{
    bitarray_clean_bit(bitmap, bloque);
}

uint32_t *obtenerPunterosIndirectos(t_fcb *fcb)
{
    if (fcb->puntero_indirecto == 0)
        return NULL;
    uint32_t cpb = get_cpb();
    uint32_t *punterosIndirectos = malloc(sizeof(uint32_t) * cpb);
    for (int i = 0; i < get_cpb(); i++)
        punterosIndirectos[i] = 0;
    uint32_t i = 0;
    uint32_t pIndirecto = -1;
    while (pIndirecto != 0 && i < cpb)
    {
        uint32_t offset = superbloque->block_size * fcb->puntero_indirecto + i * sizeof(uint32_t);
        memcpy(&pIndirecto, blocks + offset, sizeof(uint32_t));
        punterosIndirectos[i] = pIndirecto;
        i++;
    }
    sleep(config_fs->retardo_acceso_bloque / 1000);
    return punterosIndirectos;
}

uint32_t cantidadPunterosIndirectos(uint32_t *punterosIndirectos)
{
    uint32_t contador = 0;
    if (punterosIndirectos == NULL)
        return -1;
    for (int i = 0; i < get_cpb(); i++)
        if (punterosIndirectos[i] != 0)
            contador++;
        else
            return contador;
    return contador;
}
