#ifndef FILESYSTEM_VARS_H
#define FILESYSTEM_VARS_H

#include <stdint.h>
#include <stdbool.h>

#include "utils.h"

#include <commons/log.h>

extern t_log *logger_main;
extern t_log *logger_aux;

extern t_config_filesystem *config;

extern int memory_socket;

#endif /* FILESYSTEM_VARS_H */