#include "instructions.h"

t_pcontexto_desalojo *stop_exec(t_pcontexto *contexto, t_instruccion *instruccionListaParaEjecutar, t_status_code status_code)
{
    ejecutando = false;
    t_pcontexto_desalojo *contexto_desalojo = copy_pcontexto(contexto);
    contexto_desalojo->motivo_desalojo = instruccion_duplicate(instruccionListaParaEjecutar);
    contexto_desalojo->status_code = status_code;
    return contexto_desalojo;
}

void SET(char *registro_char, char *valor_char)
{ // setea el registro pasado por param a el valor pasado por parametro
    void *registro = get_register(registro_char);
    memcpy(registro, valor_char, string_length(valor_char)); // siguen siendo chars, solo que ahora no tienen el '\0' para chequear el fin de cadena
}

// mem -> reg -- lectura
// Lee el valor de memoria correspondiente a la Direccion Logica y lo almacena en el Registro
char *MOV_IN(char *registro, char *direccion_fisica, uint32_t pid)
{
    void *reg = get_register(registro);
    int tam_reg = get_sizeof_register(registro);

    // creo info_read para peticion de lectura a memoria
    t_info_read *info_read = malloc(sizeof(t_info_read));
    info_read->base_address = atoi(direccion_fisica); // PREGUNTA - aca hay que hacer un string_duplicate o esta bien asi???????????????????????????????????
    info_read->size = tam_reg;

    bool res = send_info_read(socket_client_memoria, info_read, logger_aux);
    if(!res)
        log_error(logger_aux, "No se pudo enviar la instruccion de MOV_IN a memoria");
    free(info_read);

    t_package *package = get_package(socket_client_memoria, logger);

    t_info *info;
    char *value = NULL;

    switch (package->operation_code)
    {
    case INFO:
        value = malloc(tam_reg + 1);
        *(value + tam_reg) = '\0';
        info = get_info(package);
        memcpy(reg, info->data, tam_reg);
        memcpy(value, info->data, tam_reg);
        info_destroy(info);
        break;
    default:
        log_error(logger_aux, "Recibido error de memoria en el MOV_IN (STATUS_CODE) - SEGMENTATION_FAULT");
        break;
    }
    package_destroy(package);

    return value;
}

// reg -> mem -- escritura
// Lee el valor del Registro y lo escribe en la direccion fisica de memoria obtenida a partir de la Direccion Logica
char *MOV_OUT(char *direccion_fisica, char *registro, uint32_t pid)
{
    void *reg = get_register(registro);
    int tam_reg = get_sizeof_register(registro);

    char *valor_reg = malloc(tam_reg + 1);
    *(valor_reg + tam_reg) = '\0';
    memcpy(valor_reg, reg, tam_reg); // creo un valor vhar para hacer el log despues

    // creo el t_info_write para mandarle la peticion de escritura a memoria
    t_info_write *info_write = malloc(sizeof(t_info_write));
    info_write->info = malloc(sizeof(t_info));
    info_write->base_address = atoi(direccion_fisica);
    info_write->info->data = malloc(tam_reg);
    memcpy(info_write->info->data, reg, tam_reg);
    info_write->info->size = tam_reg;
    
    bool res = send_info_write(socket_client_memoria, info_write, logger_aux);
    if(!res)
        log_error(logger_aux, "No se pudo enviar la instruccion de MOV_OUT a memoria");

    info_write_destroy(info_write);
    
    t_package *package = get_package(socket_client_memoria, logger);

    switch (package->operation_code)
    {
    case STATUS_CODE:
        t_status_code status_code;
        status_code = get_status_code(package);
        if(status_code == SUCCESS) {
            log_info(logger_aux, "Se pudo escribir en memoria en el MOV_OUT");
        } else {
            log_error(logger_aux, "Recibido error de memoria en el MOV_OUT (STATUS_CODE) - SEGMENTATION_FAULT");
            free(valor_reg);
            valor_reg = NULL;
        }
        break;
    default:
        log_error(logger_aux, "No se pudo obtener el OK de memoria en el MOV_OUT");
        free(valor_reg);
        valor_reg = NULL;
        break;
    }

    package_destroy(package);

    return valor_reg;
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
    registers_duplicate(contexto->registers, contexto_desalojo->registers);
    contexto_desalojo->instructions = copy_instructions_list(contexto->instructions);
    return contexto_desalojo;
}

// devuelve mallockeado
t_list *copy_instructions_list(t_list *instructions)
{
    t_list *new_list = list_create();
    for (int i = 0; i < list_size(instructions); i++)
    {
        t_instruccion *instruction = instruccion_duplicate(((t_instruccion *)list_get(instructions, i)));
        list_add(new_list, instruction);
    }
    return new_list;
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
