#include "instructions.h"

t_pcontexto_desalojo *stop_exec(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    ejecutando = false; // TODO HOY

    // crea t_pcon_desalojo
    // copia el contexto y lo devuelve
    free(instruccionListaParaEjecutar);
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

t_pcontexto_desalojo *I_O(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_OPEN(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_CLOSE(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_SEEK(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_READ(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_WRITE(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_TRUNCATE(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *WAIT(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *SIGNAL(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *CREATE_SEGMENT(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *DELETE_SEGMENT(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *YIELD(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) { // desaloja voluntariamente el proceso de la CPU
    return stop_exec(contexto, instruccionListaParaEjecutar);
} 

t_pcontexto_desalojo *EXIT(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) { // instruccion de finalizacion de proceso
    return stop_exec(contexto, instruccionListaParaEjecutar);
}


// aux
void *get_register(char *register_char) {
    if(strcmp(register_char, "AX") == 0) {
        return cpu_registers->AX;
    } else if(strcmp(register_char, "BX") == 0) {
        return cpu_registers->BX;
    } else if(strcmp(register_char, "CX") == 0) {
        return cpu_registers->CX;
    } else if(strcmp(register_char, "DX") == 0) {
        return cpu_registers->DX;
    } else if(strcmp(register_char, "EAX") == 0) {
        return cpu_registers->EAX;
    } else if(strcmp(register_char, "EBX") == 0) {
        return cpu_registers->EBX;
    } else if(strcmp(register_char, "ECX") == 0) {
        return cpu_registers->ECX;
    } else if(strcmp(register_char, "EDX") == 0) {
        return cpu_registers->EDX;
    } else if(strcmp(register_char, "RAX") == 0) {
        return cpu_registers->RAX;
    } else if(strcmp(register_char, "RBX") == 0) {
        return cpu_registers->RBX;
    } else if(strcmp(register_char, "RCX") == 0) {
        return cpu_registers->RCX;
    } else if(strcmp(register_char, "RDX") == 0) {
        return cpu_registers->RDX;
    }
}