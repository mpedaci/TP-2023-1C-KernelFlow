#ifndef PRINT_INFORMATION_H
#define PRINT_INFORMATION_H

#include "memory_vars.h"
#include "utils.h"

void print_menu();
void print_config();
void print_segments_tables(t_segments_table *segments_table);
void print_free_space_table();
void print_all_memory();
void print_status_memory();
#endif /* PRINT_INFORMATION_H */