#include "instructions.h"

t_pcontexto_desalojo *stop_exec(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    ejecutando = false;
    t_pcontexto_desalojo *contexto_desalojo = copy_pcontexto(contexto);
    // contexto_desalojo->motivo_desalojo = new_instruction(instruccionListaParaEjecutar->identificador, instruccionListaParaEjecutar->parametros);
    contexto_desalojo->motivo_desalojo = instruccionListaParaEjecutar;
    return contexto_desalojo;
}


void SET(char* registro_char, char* valor_char) { // setea el registro pasado por param a el valor pasado por parametro
    void *registro = get_register(registro_char);
    memcpy(registro, valor_char, string_length(valor_char)); // siguen siendo chars, solo que ahora no tienen el '\0' para chequear el fin de cadena
}

// mem -> reg
// Lee el valor de memoria correspondiente a la Dirección Lógica y lo almacena en el Registro
char *MOV_IN(char *registro, uint32_t direccion_fisica) {
    // void *reg = get_register(registro);
    // TODO
    return ""; // return valor leido de memoriaf
}

// reg -> mem
// Lee el valor del Registro y lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica
char *MOV_OUT(uint32_t direccion_fisica, char *registro) {
    // void * reg = get_register(registro);
    // TODO 
    return "";// return valor leido de memoria
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
    } else {
        return NULL;
    }
}

void instruction_destroyer(t_instruccion *instruccion) {
    list_destroy_and_destroy_elements(instruccion->parametros, free);
    free(instruccion);
}

// devuelve mallockeado, sin settear motivo_desalojo
t_pcontexto_desalojo *copy_pcontexto(t_pcontexto *contexto) {
    t_pcontexto_desalojo *contexto_desalojo = malloc(sizeof(t_pcontexto_desalojo));

    // memcpy(&contexto_desalojo->pid, &contexto->pid, sizeof(uint32_t));
    contexto_desalojo->pid = contexto->pid;

    // memcpy(&contexto_desalojo->program_counter, &contexto->program_counter, sizeof(uint32_t));
    contexto_desalojo->program_counter = contexto->program_counter;

    contexto_desalojo->registers = init_registers();
    copy_registers(contexto_desalojo->registers, contexto->registers);

    // contexto_desalojo->instructions = copy_instructions_list(contexto->instructions);
    contexto_desalojo->instructions = contexto->instructions;

    return contexto_desalojo;
}

// NO mallockea, solo copia
void copy_registers(t_registers *dest, t_registers *src) {
    memcpy(dest->AX, src->AX, 4);
    memcpy(dest->BX, src->BX, 4);
    memcpy(dest->CX, src->CX, 4);
    memcpy(dest->DX, src->DX, 4);

    memcpy(dest->EAX, src->EAX, 8);
    memcpy(dest->EBX, src->EBX, 8);
    memcpy(dest->ECX, src->ECX, 8);
    memcpy(dest->EDX, src->EDX, 8);

    memcpy(dest->RAX, src->RAX, 16);
    memcpy(dest->RBX, src->RBX, 16);
    memcpy(dest->RCX, src->RCX, 16);
    memcpy(dest->RDX, src->RDX, 16);
}

// devuelve mallockeado
t_list *copy_instructions_list(t_list *instructions) {
    t_list *new_list = list_create();
    t_instruccion *instruction;

    for(int i=0; i<list_size(instructions); i++) {
        instruction = new_instruction(((t_instruccion *)list_get(instructions, i)));
        list_add(new_list, instruction);
    }

    return new_list;
}

// devuelve mallockeado
t_instruccion *new_instruction(t_instruccion *instruccion) {
    t_instruccion *new_instruction = malloc(sizeof(t_instruccion));
    new_instruction->identificador = instruccion->identificador;
    new_instruction->cant_parametros = instruccion->cant_parametros;
    new_instruction->parametros = list_create();
    for(int i=0; i<instruccion->cant_parametros; i++) {
        list_add(new_instruction->parametros, string_duplicate(list_get(instruccion->parametros, i)));
    }
    for(int i=0; i<4; i++) {
        new_instruction->p_length[i] = instruccion->p_length[i];
    }
    return new_instruction;
}
