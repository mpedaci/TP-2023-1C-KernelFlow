#ifndef MEMORY_VARS_H
#define MEMORY_VARS_H

#include <stdint.h>
#include <stdbool.h>
#include <commons/log.h>
#include "server.h"
#include "utils.h"

// Loggers
extern t_log *logger_main;
extern t_log *logger_aux;

// Config
extern t_config_memoria *config;

// Estructuras
extern void *memory_space;
extern t_bitarray *free_space_table;

extern t_segment *segment_0;
extern t_list *all_segments_tables;

#endif /* MEMORY_VARS_H */