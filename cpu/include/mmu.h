#ifndef MMU_H
#define MMU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "utils.h"
#include "cpu_vars.h"
#include "types.h"

<<<<<<< HEAD
int get_num_segmento(char *dir_logica);

int get_desplazamiento_segmento(char *dir_logica);

char *get_direccion_fisica(char *dir_logica, t_list *segmentos);
=======
int get_num_segmento(int dir_logica);

int get_desplazamiento_segmento(int dir_logica);

int get_direccion_fisica(int dir_logica, t_list *segmentos);
>>>>>>> cpu

t_segment *get_by_num_segmento(int num_segmento, t_list *segmentos);

#endif