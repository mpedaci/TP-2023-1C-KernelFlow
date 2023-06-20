#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "client_handle.h"
#include "parser.h"
#include "types_controllers.h"

t_log* logger_console; 

char* logger_console_path = "./logs/consola.log";

#endif /* CONSOLA_H_ */