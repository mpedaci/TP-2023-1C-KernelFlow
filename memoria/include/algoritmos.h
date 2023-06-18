#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "utils.h"
#include "memory_vars.h"
#include "bitarray.h"

uint32_t FIRST_FIT(t_segment* segment);

uint32_t BEST_FIT(t_segment* segment);

uint32_t WORST_FIT(t_segment* segment);

#endif /* ALGORITMOS_H */