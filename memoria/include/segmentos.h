#ifndef SEGMENTOS_H
#define SEGMENTOS_H

#include "utils.h"

t_segment *create_segment(int id, int size);
t_segment *new_segment(int id, int size);
void delete_segment(int pid, t_segment *segment);
bool is_malloc_possible(int size);
int get_base_adress(t_segment *segment);

t_segments_table *create_segments_table(int pid);
void add_segment_to_table(int pid, t_segment *segment);
void delete_segments_table(t_segments_table *segments_table);

#endif /* SEGMENTOS_H */