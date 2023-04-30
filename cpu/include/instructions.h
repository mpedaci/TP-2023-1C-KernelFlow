#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "cpu.h"


void SET(char* registro, char* valor);

void YIELD(t_execution_context* context);

void EXIT(t_execution_context* context);


#endif