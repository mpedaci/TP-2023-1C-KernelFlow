#ifndef COMPACTACION_H
#define COMPACTACION_H

#include "segmentos.h"

void compact_memory();

void free_gaps_info(int *first_zero, int *last_one, int *first_one_after_first_zero);
int find_first_zero();
int find_last_one();
int find_first_one_after_first_zero(int *first_zero);

#endif /* COMPACTACION_H */