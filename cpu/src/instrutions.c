#include "instructions.h"

t_pcontexto_desalojo *stop_exec(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    ejecutando = false;

    t_pcontexto_desalojo *contexto_desalojo = copy_pcontexto(contexto);
    contexto_desalojo->motivo_desalojo = copy_instruction(instruccionListaParaEjecutar);

    instruction_destroyer(instruccionListaParaEjecutar);

    return contexto_desalojo;
}


void SET(char* registro_char, char* valor_char) { // setea el registro pasado por param a el valor pasado por parametro
    void *registro = get_register(registro_char);
    memcpy(registro, valor_char, string_length(valor_char)); // siguen siendo chars, solo que ahora no tienen el '\0' para chequear el fin de cadena
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

void instruction_destroyer(t_instruccion *instruccion) {
    free(instruccion->parametros);
    free(instruccion);
}

// devuelve mallockeado, sin settear motivo_desalojo
t_pcontexto_desalojo *copy_pcontexto(t_pcontexto *contexto) {
    t_pcontexto_desalojo *contexto_desalojo = malloc(sizeof(t_pcontexto_desalojo));

    memcpy(&contexto_desalojo->pid, &contexto->pid, sizeof(uint32_t));
    memcpy(&contexto_desalojo->program_counter, &contexto->program_counter, sizeof(uint32_t));

    init_registers(contexto_desalojo->registers);
    copy_registers(contexto_desalojo->registers, contexto->registers);

    contexto_desalojo->instructions = copy_instructions_list(contexto->instructions);

    return contexto_desalojo;
}

// NO mallockea, solo copia
void copy_registers(t_registers *dest, t_registers *src) {
    memcpy(dest->AX, src->AX, sizeof(4));
    memcpy(dest->BX, src->BX, sizeof(4));
    memcpy(dest->CX, src->CX, sizeof(4));
    memcpy(dest->DX, src->DX, sizeof(4));

    memcpy(dest->EAX, src->EAX, sizeof(8));
    memcpy(dest->EBX, src->EBX, sizeof(8));
    memcpy(dest->ECX, src->ECX, sizeof(8));
    memcpy(dest->EDX, src->EDX, sizeof(8));

    memcpy(dest->RAX, src->RAX, sizeof(16));
    memcpy(dest->RBX, src->RBX, sizeof(16));
    memcpy(dest->RCX, src->RCX, sizeof(16));
    memcpy(dest->RDX, src->RDX, sizeof(16));
}

// devuelve mallockeado
t_list *copy_instructions_list(t_list *instructions) {
    t_list *new_list = list_create();
    t_instruccion *instruction;

    for(int i=0; i<list_size(instructions); i++) {
        instruction = copy_instruction(list_get(instructions, i));
        list_add(new_list, instruction);
    }

    return new_list;
}

// devuelve mallockeado
t_instruccion *copy_instruction(t_instruccion *instruction) {
    t_instruccion* instruction_new = malloc(sizeof(t_instruccion));

    memcpy(&instruction_new->identificador, &instruction->identificador, sizeof(t_identificador));
    memcpy(&instruction_new->cant_parametros, &instruction->cant_parametros, sizeof(uint32_t));
    memcpy(&instruction_new->p1_length, &instruction->p1_length, sizeof(uint32_t));
    memcpy(&instruction_new->p2_length, &instruction->p2_length, sizeof(uint32_t));
    memcpy(&instruction_new->p3_length, &instruction->p3_length, sizeof(uint32_t));
    memcpy(&instruction_new->p4_length, &instruction->p4_length, sizeof(uint32_t));

    instruction_new->parametros = malloc(sizeof(char*) * instruction_new->cant_parametros);

    add_params_instruction(instruction_new, instruction->parametros);

    return instruction_new;
}

void add_params_instruction(t_instruccion *instruction, char** params) {
    for(int i=0; i<instruction->cant_parametros; i++) {
        memcpy(instruction->parametros[i], params[i], sizeof(string_length(params[i])));
    }
}