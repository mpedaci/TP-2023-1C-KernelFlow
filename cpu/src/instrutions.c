#include "instructions.h"

t_pcontexto_desalojo *stop_exec(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar, t_status_code status_code)
{
    ejecutando = false;
    t_pcontexto_desalojo *contexto_desalojo = copy_pcontexto(contexto);
    contexto_desalojo->motivo_desalojo = new_instruction(instruccionListaParaEjecutar);
    contexto_desalojo->status_code = status_code;
    return contexto_desalojo;
}

void SET(char *registro_char, char *valor_char)
{ // setea el registro pasado por param a el valor pasado por parametro
    void *registro = get_register(registro_char);
    memcpy(registro, valor_char, string_length(valor_char)); // siguen siendo chars, solo que ahora no tienen el '\0' para chequear el fin de cadena
}

// mem -> reg -- lectura
// Lee el valor de memoria correspondiente a la Dirección Lógica y lo almacena en el Registro
void MOV_IN(char *registro, char *direccion_fisica, uint32_t pid)
{
    void *reg = get_register(registro);
    int tam_reg = get_sizeof_register(registro);

    t_list *params = list_create();
    list_add(params, direccion_fisica);
    list_add(params, string_itoa(tam_reg));

    t_instruccion *instruccion_a_mandar = create_new_instruction(I_MOV_IN, params);
    t_pid_instruccion *pid_instruccion = malloc(sizeof(t_pid_instruccion));
    pid_instruccion->pid = pid;
    pid_instruccion->instruccion = instruccion_a_mandar;
    bool res = send_pid_instruccion(socket_client_memoria, pid_instruccion, logger_aux);
    if(!res)
        log_error(logger_aux, "No se pudo enviar la instruccion de MOV_IN a memoria");
    
    instruccion_destroy(instruccion_a_mandar);
    free(pid_instruccion);

    t_package *package = get_package(socket_client_memoria, logger);

    t_data *data;
    if(package->operation_code == DATA) {
        data = get_data(package);
        memcpy(reg, data->value, tam_reg); // copia solo el tamanio del registro, si la data es mas grande que el tam del registro, se pierde
        free(data->value);
        free(data);
    } else {
        log_error(logger_aux, "No se pudo obtener el valor de memoria en el MOV_IN");
    }

    package_destroy(package);
}

// reg -> mem -- escritura
// Lee el valor del Registro y lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica
void MOV_OUT(char *direccion_fisica, char *registro, uint32_t pid)
{
    void *reg = get_register(registro);
    int tam_reg = get_sizeof_register(registro);

    char *valor_reg = malloc(tam_reg + 1);

    memcpy(valor_reg, reg, tam_reg);

    t_list *params = list_create();
    list_add(params, direccion_fisica);
    list_add(params, valor_reg);

    t_instruccion *instruccion_a_mandar = create_new_instruction(I_MOV_OUT, params);
    t_pid_instruccion *pid_instruccion = malloc(sizeof(t_pid_instruccion));
    pid_instruccion->pid = pid;
    pid_instruccion->instruccion = instruccion_a_mandar;
    bool res = send_pid_instruccion(socket_client_memoria, pid_instruccion, logger_aux);
    if(!res)
        log_error(logger_aux, "No se pudo enviar la instruccion de MOV_OUT a memoria");
    
    instruccion_destroy(instruccion_a_mandar);
    free(pid_instruccion);

    t_package *package = get_package(socket_client_memoria, logger);

    t_status_code status_code;
    if(package->operation_code == STATUS_CODE) {
        status_code = get_status_code(package);
        if(status_code == SUCCESS) {
            log_info(logger_aux, "Se pudo escribir en memoria en el MOV_OUT");
        } else {
            log_error(logger_aux, "No se pudo escribir en memoria en el MOV_OUT");
        }
    } else {
        log_error(logger_aux, "No se pudo obtener el OK de memoria en el MOV_OUT");
    }

    package_destroy(package);
}

t_pcontexto_desalojo *I_O(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *F_OPEN(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *F_CLOSE(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *F_SEEK(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *F_READ(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *F_WRITE(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *F_TRUNCATE(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *WAIT(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *SIGNAL(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *CREATE_SEGMENT(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *DELETE_SEGMENT(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *YIELD(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{ // desaloja voluntariamente el proceso de la CPU
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
}

t_pcontexto_desalojo *EXIT(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar)
{ // instruccion de finalizacion de proceso
    return stop_exec(contexto, instruccionListaParaEjecutar, SUCCESS);
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
