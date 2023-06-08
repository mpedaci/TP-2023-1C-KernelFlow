#include "mmu.h"

// este mmu es una cagada, para mi no esta bien
// esto es para paginacion, no para segmentacion

int get_num_segmento(int dir_logica)
{
    int tam_max_segmento = atoi(config->tam_max_segmento);
    return (int)floor(dir_logica / tam_max_segmento);
}

int get_desplazamiento_segmento(int dir_logica)
{
    int tam_max_segmento = atoi(config->tam_max_segmento);
    return dir_logica % tam_max_segmento;
}

int get_direccion_fisica(int dir_logica, t_list *segmentos)
{
    // agarrar el segmento que corresponde con el numero de segmento
    // agarrar el desplazamiento que corresponde con la direccion logica
    // sumar el desplazamiento al inicio del segmento
    // devolver la direccion fisica
    int num_segmento = get_num_segmento(dir_logica);
    int desplazamiento = get_desplazamiento_segmento(dir_logica);
    t_segment *segmento = get_by_num_segmento(num_segmento, segmentos);
    int inicio_segmento = segmento->base_address;
    return inicio_segmento + desplazamiento;
}

t_segment *get_by_num_segmento(int num_segmento, t_list *segmentos)
{
    t_segment *segmento;
    for (int i = 0; i < list_size(segmentos); i++)
    {
        segmento = list_get(segmentos, i);
        if (segmento->id == num_segmento)
            return segmento;
    }
    return NULL;
}