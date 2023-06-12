#include "instructions.h"

t_pcontexto_desalojo *stop_exec(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    ejecutando = false;
    t_pcontexto_desalojo *contexto_desalojo = copy_pcontexto(contexto);
    contexto_desalojo->motivo_desalojo = new_instruction(instruccionListaParaEjecutar);
    return contexto_desalojo;
}

void SET(char *registro_char, char *valor_char)
{ // setea el registro pasado por param a el valor pasado por parametro
    void *registro = get_register(registro_char);
    memcpy(registro, valor_char, string_length(valor_char)); // siguen siendo chars, solo que ahora no tienen el '\0' para chequear el fin de cadena
}

// mem -> reg -- lectura
// Lee el valor de memoria correspondiente a la Dirección Lógica y lo almacena en el Registro
char *MOV_IN(char *registro, char *direccion_fisica)
{
    void *reg = get_register(registro);

    t_list *params = list_create();
    list_add(params, direccion_fisica);

    t_instruccion *instruccion_a_mandar = create_new_instruction(I_MOV_IN, params);
    bool res = send_instruccion(socket_client_memoria, instruccion_a_mandar, logger_aux);
    if(!res)
        log_error(logger_aux, "No se pudo enviar la instruccion de MOV_IN a memoria");
    
    instruction_destroyer(instruccion_a_mandar);

    t_package *package = get_package(socket_client_memoria, logger);

    t_data *data;
    if(package->operation_code == DATA) {
        data = get_data(package);
    } else {
        log_error(logger_aux, "No se pudo obtener el valor de memoria en el MOV_IN");
    }

    package_destroy(package);

    memcpy(reg, data->value, get_sizeof_register(registro)); // copia solo el tamanio del registro, si la data es mas grande que el tam del registro, se pierde

    return data->value;
}

// reg -> mem -- escritura
// Lee el valor del Registro y lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica
char *MOV_OUT(char *direccion_fisica, char *registro)
{
    void *reg = get_register(registro);

    char *valor_reg = NULL; // puede que haya que hacer un malloc, por ahora no tira error en el make - CHECKEAR

    memcpy(valor_reg, reg, get_sizeof_register(registro));

    t_list *params = list_create();
    list_add(params, direccion_fisica);
    list_add(params, valor_reg);

    t_instruccion *instruccion_a_mandar = create_new_instruction(I_MOV_OUT, params);
    bool res = send_instruccion(socket_client_memoria, instruccion_a_mandar, logger_aux);
    if(!res)
        log_error(logger_aux, "No se pudo enviar la instruccion de MOV_OUT a memoria");
    
    instruction_destroyer(instruccion_a_mandar);

    t_package *package = get_package(socket_client_memoria, logger);

    t_data *data;
    if(package->operation_code == DATA) {
        data = get_data(package);
    } else {
        log_error(logger_aux, "No se pudo obtener el valor de memoria en el MOV_OUT");
    }

    package_destroy(package);

    if(strcmp(data->value, "OK") != 0)
        log_error(logger_aux, "No se pudo escribir en memoria en el MOV_OUT (no se recibio OK)");

    return data->value;
}

t_pcontexto_desalojo *I_O(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_OPEN(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_CLOSE(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_SEEK(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_READ(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_WRITE(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *F_TRUNCATE(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *WAIT(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *SIGNAL(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *CREATE_SEGMENT(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *DELETE_SEGMENT(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *YIELD(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{ // desaloja voluntariamente el proceso de la CPU
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

t_pcontexto_desalojo *EXIT(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{ // instruccion de finalizacion de proceso
    return stop_exec(contexto, instruccionListaParaEjecutar);
}

// aux
void *get_register(char *register_char)
{
    if (strcmp(register_char, "AX") == 0)
    {
        return cpu_registers->AX;
    }
    else if (strcmp(register_char, "BX") == 0)
    {
        return cpu_registers->BX;
    }
    else if (strcmp(register_char, "CX") == 0)
    {
        return cpu_registers->CX;
    }
    else if (strcmp(register_char, "DX") == 0)
    {
        return cpu_registers->DX;
    }
    else if (strcmp(register_char, "EAX") == 0)
    {
        return cpu_registers->EAX;
    }
    else if (strcmp(register_char, "EBX") == 0)
    {
        return cpu_registers->EBX;
    }
    else if (strcmp(register_char, "ECX") == 0)
    {
        return cpu_registers->ECX;
    }
    else if (strcmp(register_char, "EDX") == 0)
    {
        return cpu_registers->EDX;
    }
    else if (strcmp(register_char, "RAX") == 0)
    {
        return cpu_registers->RAX;
    }
    else if (strcmp(register_char, "RBX") == 0)
    {
        return cpu_registers->RBX;
    }
    else if (strcmp(register_char, "RCX") == 0)
    {
        return cpu_registers->RCX;
    }
    else if (strcmp(register_char, "RDX") == 0)
    {
        return cpu_registers->RDX;
    }
    else
    {
        return NULL;
    }
}

void instruction_destroyer(t_instruccion *instruccion)
{
    list_destroy_and_destroy_elements(instruccion->parametros, free);
    free(instruccion);
}

// devuelve mallockeado, sin settear motivo_desalojo
t_pcontexto_desalojo *copy_pcontexto(t_pcontexto *contexto)
{
    t_pcontexto_desalojo *contexto_desalojo = malloc(sizeof(t_pcontexto_desalojo));
    contexto_desalojo->pid = contexto->pid;
    contexto_desalojo->program_counter = contexto->program_counter;
    contexto_desalojo->registers = init_registers();
    copy_registers(contexto_desalojo->registers, contexto->registers);
    contexto_desalojo->instructions = copy_instructions_list(contexto->instructions);
    return contexto_desalojo;
}

// NO mallockea, solo copia
void copy_registers(t_registers *dest, t_registers *src)
{
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
t_list *copy_instructions_list(t_list *instructions)
{
    t_list *new_list = list_create();
    for (int i = 0; i < list_size(instructions); i++)
    {
        t_instruccion *instruction = new_instruction(((t_instruccion *)list_get(instructions, i)));
        list_add(new_list, instruction);
    }
    return new_list;
}

// devuelve mallockeado
t_instruccion *new_instruction(t_instruccion *instruccion)
{
    t_instruccion *new_instruction = malloc(sizeof(t_instruccion));
    new_instruction->identificador = instruccion->identificador;
    new_instruction->cant_parametros = instruccion->cant_parametros;
    new_instruction->parametros = list_create();
    for (int i = 0; i < instruccion->cant_parametros; i++)
    {
        char *param = string_duplicate(list_get(instruccion->parametros, i));
        list_add(new_instruction->parametros, param);
    }
    for (int i = 0; i < 4; i++)
        new_instruction->p_length[i] = instruccion->p_length[i];
    return new_instruction;
}

void add_param_size_to_instruction(t_list *parametros, t_instruccion *instruccion)
{
    int i = 0;
    while (i < instruccion->cant_parametros)
    {
        char *param = list_get(parametros, i);
        instruccion->p_length[i] = strlen(param) + 1;
        i++;
    }
}

t_instruccion *create_new_instruction(t_identificador identificador, t_list *parametros)
{
    t_instruccion *tmp = malloc(sizeof(t_instruccion));
    tmp->identificador = identificador;
    tmp->cant_parametros = list_size(parametros);
    tmp->parametros = parametros;
    for (size_t i = 0; i < 4; i++)
        tmp->p_length[i] = 0;
    add_param_size_to_instruction(parametros, tmp);
    return tmp;
}