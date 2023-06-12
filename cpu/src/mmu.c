#include "mmu.h"

// este mmu es una cagada, para mi no esta bien
// esto es para paginacion, no para segmentacion

int get_num_segmento(char *dir_logica)
{
    // int tam_max_segmento = atoi(config->tam_max_segmento);
    // return (int)floor(atoi(dir_logica) / tam_max_segmento);

    /*
        esto deberia ser asi

        seteamos como default que los dos primeros n numeros de la direccion logica son el numero de segmento

        agarramos los primeros n numeros de la direccion logica y los convertimos a int

        y ya tenemos el nuemro de segmento, luego los otros numeros son el desplazamiento
    */

    return 1;
}

int get_desplazamiento_segmento(char *dir_logica)
{
    // int tam_max_segmento = atoi(config->tam_max_segmento);
    // return atoi(dir_logica) % tam_max_segmento;
    return 0;
}

char *get_direccion_fisica(char *dir_logica, t_list *segments)
{
    // agarrar el segmento que corresponde con el numero de segmento
    // agarrar el desplazamiento que corresponde con la direccion logica
    // sumar el desplazamiento al inicio del segmento
    // devolver la direccion fisica
    int num_segmento = get_num_segmento(dir_logica);
    int desplazamiento = get_desplazamiento_segmento(dir_logica);
    t_segment *segmento = get_by_num_segmento(num_segmento, segments);
    int inicio_segmento = segmento->base_address;
    return string_itoa(inicio_segmento + desplazamiento);
}

t_segment *get_by_num_segmento(int num_segment, t_list *segments)
{
    t_segment *segmento;
    for (int i = 0; i < list_size(segments); i++)
    {
        segmento = list_get(segments, i);
        if (segmento->id == num_segment)
            return segmento;
    }
    return NULL;
}