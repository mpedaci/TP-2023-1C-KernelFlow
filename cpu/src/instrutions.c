#include "instructions.h"


void SET(char* registro, char* valor) {
    registro = string_duplicate(valor);
}


void YIELD(t_execution_context* context) { // desaloja voluntariamente el proceso de la CPU
    send_context_kernel(context); 
}


void EXIT(t_execution_context* context) { // instruccion de finalizacion de proceso
    send_context_kernel(context);
}