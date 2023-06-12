#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include "compactacion.h"

void start_memory(t_config_memoria* config);
void end_memory();
t_bitarray* create_free_space_table(size_t memory_size);

#endif /* ESTRUCTURAS_H */