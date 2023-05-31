#include "instructions.h"

t_pcontexto_desalojo *stop_exec(t_pcontexto *contexto, t_instruccion* instruccionListaParaEjecutar) {
    ejecutando = false;

    t_pcontexto_desalojo *contexto_desalojo = copy_pcontexto(contexto);
    contexto_desalojo->motivo_desalojo = copy_instruction(instruccionListaParaEjecutar);

    return contexto_desalojo;
}


void SET(char* registro_char, char* valor_char) { // setea el registro pasado por param a el valor pasado por parametro
    void *registro = get_register(registro_char);
    memcpy(registro, valor_char, string_length(valor_char)); // siguen siendo chars, solo que ahora no tienen el '\0' para chequear el fin de cadena
}

// mem -> reg
// Lee el valor de memoria correspondiente a la Dirección Lógica y lo almacena en el Registro
char *MOV_IN(char *registro, uint32_t direccion_fisica) {
    void *reg = get_register(registro);
    // TODO
    return ""; // return valor leido de memoria
}

// reg -> mem
// Lee el valor del Registro y lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica
char *MOV_OUT(uint32_t direccion_fisica, char *registro) {
    void * reg = get_register(registro);
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

    contexto_desalojo->registers = init_registers();
    copy_registers(contexto_desalojo->registers, contexto->registers);

    contexto_desalojo->instructions = copy_instructions_list(contexto->instructions);

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
        instruction = copy_instruction(list_get(instructions, i));
        list_add(new_list, instruction);
    }

    return new_list;
}

// devuelve mallockeado
t_instruccion *copy_instruction(t_instruccion *instruction) {
    t_list *params = list_create();
    for(int i=0; i<instruction->cant_parametros; i++) {
        list_add(params, instruction->parametros[i]);
    }

    t_instruccion* instruction_new = new_instruction(instruction->identificador, params);

    list_destroy(params);

    return instruction_new;
}

t_instruccion *new_instruction(t_identificador identificador, t_list *parametros)
{
    t_instruccion *tmp = malloc(sizeof(t_instruccion));

    tmp->identificador = identificador;
    if (parametros == NULL)
    {
        tmp->cant_parametros = 0;
        tmp->parametros = NULL;
    }
    else
    {
        tmp->cant_parametros = list_size(parametros);
        tmp->parametros = malloc(sizeof(char *) * tmp->cant_parametros);
        add_param_to_instruction(parametros, tmp);
    }

    return tmp;
}

void add_param_to_instruction(t_list *parametros, t_instruccion *instruccion)
{
    if (parametros != NULL)
        for(int i=0; i < instruccion->cant_parametros; i++)
        {
            instruccion->parametros[i] = list_get(parametros, i);
        }
    instruccion->p1_length = 1;
    instruccion->p2_length = 1;
    instruccion->p3_length = 1;
    instruccion->p4_length = 1;
    instruccion->p1_length = strlen(instruccion->parametros[0]) + 1;
    if (instruccion->cant_parametros >= 2)
        instruccion->p2_length = strlen(instruccion->parametros[1]) + 1;
    if (instruccion->cant_parametros >= 3)
        instruccion->p3_length = strlen(instruccion->parametros[2]) + 1;
    if (instruccion->cant_parametros >= 4)
        instruccion->p4_length = strlen(instruccion->parametros[3]) + 1;
}