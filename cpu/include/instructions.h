#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "package.h"
#include "mmu.h"

void stop_exec();

void SET(void* registro, void* valor);

void YIELD();

void EXIT();


#endif