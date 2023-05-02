#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "package.h"
#include "mmu.h"

void stop_exec();

void SET(char *registro, char *valor);

void MOV_IN();

void MOV_OUT();

void I_O();

void F_OPEN();

void F_CLOSE();

void F_SEEK();

void F_READ();

void F_WRITE();

void F_TRUNCATE();

void WAIT();

void SIGNAL();

void CREATE_SEGMENT();

void DELETE_SEGMENT();

void YIELD();

void EXIT();

// aux
void *get_register(char *register_char);

#endif