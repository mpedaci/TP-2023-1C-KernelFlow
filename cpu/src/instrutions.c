#include "instructions.h"

void stop_exec() {
    ejecutando = false;
}


void SET(char* registro, void* valor) { // setea el registro pasado por param a el valor pasado por parametro
    // registro = valor;

    // switch ((*registro))
    // {
    // case "AX":
        
    //     break;
    
    // default:
    //     break;
    // }

    // recibe un registro en forma de string
    // hay que hacer un switch con los registros y cuando machea buscar ese registro y setearlo al valor
    // ojo que el valor va a estar en formato string, osea sacarle el \0 y meterlo en el registro
}


void YIELD() { // desaloja voluntariamente el proceso de la CPU
    stop_exec();
} 


void EXIT() { // instruccion de finalizacion de proceso
    stop_exec();
}