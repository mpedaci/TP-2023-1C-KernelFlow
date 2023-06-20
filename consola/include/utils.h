#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct
{
    char *ip_kernel;
    char *puerto_kernel;
} t_config_console;

t_config_console *read_config(char *path, t_log *logger_console);
void end_program(t_log *logger_console, t_config_console *config, FILE *file_instructions);

#endif /* UTILS_H_ */