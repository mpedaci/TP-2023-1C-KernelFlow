#ifndef COMPACTACION_H
#define COMPACTACION_H

#include "memory_vars.h"
#include "segmentos.h"

void compact_memory();

int find_first_zero();
int find_last_one();
int find_first_one_after_first_zero(int fst_zero);

#endif /* COMPACTACION_H */