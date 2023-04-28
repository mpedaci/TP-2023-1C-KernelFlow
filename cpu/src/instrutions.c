#include "instructions.h"


void SET(void* registro, void* valor) { // setea el registro pasado por param a el valor pasado por parametro
    // registro = valor;

    // recibe un registro en formade string
    // hay que hacer un switch con los registros y cuando machea buscar ese registro y setearlo al valor
    // ojo que el valor va a estar en formato string, osea sacarle el \0 y meterlo en el registro
}


void YIELD() { // desaloja voluntariamente el proceso de la CPU
    ejecutando = false;
} 


void EXIT() { // instruccion de finalizacion de proceso
    ejecutando = false;
}