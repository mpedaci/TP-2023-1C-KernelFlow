#include "mmu.h"

int get_num_segmento(char *dir_logica)
{
    int tam_max_segmento = atoi(config->tam_max_segmento);
    return (int)floor(atoi(dir_logica) / tam_max_segmento);
}

int get_desplazamiento_segmento(char *dir_logica)
{
    int tam_max_segmento = atoi(config->tam_max_segmento);
    return atoi(dir_logica) % tam_max_segmento;
}

char *get_direccion_fisica(char *dir_logica, t_list *segments)
{
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