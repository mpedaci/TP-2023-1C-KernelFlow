#include "instructions.h"

void stop_exec() {
    ejecutando = false;
}


void SET(char* registro_char, char* valor_char) { // setea el registro pasado por param a el valor pasado por parametro
    void *registro = get_register(registro_char);
    registro = memcpy(registro, valor_char, sizeof(registro));
}

void MOV_IN() {
    // TODO (char *registro, uint32_t direccion_logica)
}

void MOV_OUT() {
    // TODO (uint32_t direccion_logica, char *registro)
}

void I_O() {
    // TODO
}

void F_OPEN() {
    // TODO
}

void F_CLOSE() {
    // TODO
}

void F_SEEK() {
    // TODO
}

void F_READ() {
    // TODO
}

void F_WRITE() {
    // TODO
}

void F_TRUNCATE() {
    // TODO
}

void WAIT() {
    // TODO
}

void SIGNAL() {
    // TODO
}

void CREATE_SEGMENT() {
    // TODO
}

void DELETE_SEGMENT() {
    // TODO
}

void YIELD() { // desaloja voluntariamente el proceso de la CPU
    stop_exec();
} 


void EXIT() { // instruccion de finalizacion de proceso
    stop_exec();
}


// aux
void *get_register(char *register_char) {
    if(strcmp((*register_char), "AX") == 0) {
        return cpu_registers->AX;
    } else if(strcmp((*register_char), "BX") == 0) {
        return cpu_registers->BX;
    } else if(strcmp((*register_char), "CX") == 0) {
        return cpu_registers->CX;
    } else if(strcmp((*register_char), "DX") == 0) {
        return cpu_registers->DX;
    } else if(strcmp((*register_char), "EAX") == 0) {
        return cpu_registers->EAX;
    } else if(strcmp((*register_char), "EBX") == 0) {
        return cpu_registers->EBX;
    } else if(strcmp((*register_char), "ECX") == 0) {
        return cpu_registers->ECX;
    } else if(strcmp((*register_char), "EDX") == 0) {
        return cpu_registers->EDX;
    } else if(strcmp((*register_char), "RAX") == 0) {
        return cpu_registers->RAX;
    } else if(strcmp((*register_char), "RBX") == 0) {
        return cpu_registers->RBX;
    } else if(strcmp((*register_char), "RCX") == 0) {
        return cpu_registers->RCX;
    } else if(strcmp((*register_char), "RDX") == 0) {
        return cpu_registers->RDX;
    }
}