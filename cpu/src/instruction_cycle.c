#include "instruction_cycle.h"

t_instruccion *fetch(t_pcontexto *contexto)
{
    t_instruccion *instruccionSiguiente = list_get(contexto->instructions, contexto->program_counter);
    contexto->program_counter++;
    return instruccionSiguiente;
}

t_instruccion *decode(t_instruccion *instruccionSiguiente)
{
    // creando instruccion lista para ejecutar
    t_instruccion *instruccionListaParaEjecutar = new_instruction(instruccionSiguiente);

    // CAMBIAR LOS PARAMETROS EN LOS LUGARES QUE SON NECESARIOS (EJ MOV_OUT)
    // ASI COMO ESTA "instruccionListaParaEjecutar" TIENE LOS MISMOS PARAMETROS QUE LA "instruccionSiguiente"
    // MMU

    switch (instruccionListaParaEjecutar->identificador)
    {
    case I_SET:
        sleep(atoi(config->retardo_instruccion)/1000); // Miliseconds -> Seconds
        break;
    case I_MOV_IN:
        // traduce mmu
        // agregarle un parametro a los parametros de la instruccion que sea el numero de segmento
        break;
    case I_MOV_OUT:
        // mmu traduce
        // params = params traducidos por mmu
        // agregarle un parametro a los parametros de la instruccion que sea el numero de segmento
        break;
    case I_F_READ:
        // mmu traduce
        break;
    case I_F_WRITE:
        // mmu traduce
        break;
    default:
        break;
    }

    return instruccionListaParaEjecutar;
}

t_pcontexto_desalojo *execute(t_instruccion *instruccionListaParaEjecutar, t_pcontexto *contexto)
{
    switch (instruccionListaParaEjecutar->identificador)
    {
    case I_SET:
        SET(list_get(instruccionListaParaEjecutar->parametros, 0), list_get(instruccionListaParaEjecutar->parametros, 1));
        break;
    case I_MOV_IN:
        MOV_IN(list_get(instruccionListaParaEjecutar->parametros, 0), atoi(list_get(instruccionListaParaEjecutar->parametros, 1)));
        // instruccionListaParaEjecutar va a tener un parametro mas que se le agrego en el decode
        // ese parametro va a servir para loggear el segundo log obligatorio. Una vez utilizado ELIMINARLO. lo mismo para MOV_OUT
        // TODO
        break;
    case I_MOV_OUT:
        MOV_OUT(atoi(list_get(instruccionListaParaEjecutar->parametros, 0)), list_get(instruccionListaParaEjecutar->parametros, 1));
        break;
    case I_I_O:
        return I_O(contexto, instruccionListaParaEjecutar);
    case I_F_OPEN:
        return F_OPEN(contexto, instruccionListaParaEjecutar);
    case I_F_CLOSE:
        return F_CLOSE(contexto, instruccionListaParaEjecutar);
    case I_F_SEEK:
        return F_SEEK(contexto, instruccionListaParaEjecutar);
    case I_F_READ:
        return F_READ(contexto, instruccionListaParaEjecutar);
    case I_F_WRITE:
        return F_WRITE(contexto, instruccionListaParaEjecutar);
    case I_F_TRUNCATE:
        return F_TRUNCATE(contexto, instruccionListaParaEjecutar);
    case I_WAIT:
        return WAIT(contexto, instruccionListaParaEjecutar);
    case I_SIGNAL:
        return SIGNAL(contexto, instruccionListaParaEjecutar);
    case I_CREATE_SEGMENT:
        return CREATE_SEGMENT(contexto, instruccionListaParaEjecutar);
    case I_DELETE_SEGMENT:
        return DELETE_SEGMENT(contexto, instruccionListaParaEjecutar);
    case I_YIELD:
        return YIELD(contexto, instruccionListaParaEjecutar);
    case I_EXIT:
        return EXIT(contexto, instruccionListaParaEjecutar);
    default:
        printf("No se pudo encontrar la instruccion :(");
        break;
    }
    return NULL;
}

t_pcontexto_desalojo *execute_instruction_cycle(t_pcontexto *contexto)
{
    t_instruccion *instruccionSiguiente = fetch(contexto);
    t_instruccion *instruccionListaParaEjecutar = decode(instruccionSiguiente);

    // loggeo la instruccion ejecutada
    char *params_string = get_params_string(instruccionListaParaEjecutar);
    char *instruction_string = get_instruction_string(instruccionListaParaEjecutar->identificador);
    log_info(logger, "PID: %d - Ejecutando: %s - %s", contexto->pid, instruction_string, params_string);

    t_pcontexto_desalojo *contexto_desalojo = execute(instruccionListaParaEjecutar, contexto);

    // log_warning(logger, "FREE 1");
    // free(params_string);
    // log_warning(logger, "FREE 2");
    // free(instruction_string);
    // log_warning(logger, "FREE 3");
    // instruction_destroyer(instruccionListaParaEjecutar);

    return contexto_desalojo;
}

t_pcontexto_desalojo *execute_process(t_pcontexto *contexto)
{
    ejecutando = true;
    t_pcontexto_desalojo *contexto_desalojo;
    while (ejecutando)
    {
        contexto_desalojo = execute_instruction_cycle(contexto);
    }
    return contexto_desalojo;
}

// aux
char *get_instruction_string(t_identificador id)
{
    switch (id)
    {
    case I_SET:
        return "SET";
    case I_MOV_IN:
        return "MOV_IN";
    case I_MOV_OUT:
        return "MOV_OUT";
    case I_I_O:
        return "I/O";
    case I_F_OPEN:
        return "F_OPEN";
    case I_F_CLOSE:
        return "F_CLOSE";
    case I_F_SEEK:
        return "F_SEEK";
    case I_F_READ:
        return "F_READ";
    case I_F_WRITE:
        return "F_WRITE";
    case I_F_TRUNCATE:
        return "F_TRUNCATE";
    case I_WAIT:
        return "WAIT";
    case I_SIGNAL:
        return "SIGNAL";
    case I_CREATE_SEGMENT:
        return "CREATE_SEGMENT";
    case I_DELETE_SEGMENT:
        return "DELETE_SEGMENT";
    case I_YIELD:
        return "YIELD";
    case I_EXIT:
        return "EXIT";
    default:
        return "Instruccion Desconocida :(";
    }
}

char *get_params_string(t_instruccion *instruction)
{
    if (instruction->cant_parametros == 0)
    {
        return "Sin parametros";
    }

    uint32_t size = instruction->cant_parametros + 1;
    for (int i = 0; i < instruction->cant_parametros; i++)
    {
        size += instruction->p_length[i];
    }

    char *params_string = malloc(size);
    memcpy(params_string, list_get(instruction->parametros, 0), instruction->p_length[0]);
    for (int i = 1; i < instruction->cant_parametros; i++)
    {
        strcat(params_string, " ");
        strcat(params_string, list_get(instruction->parametros, i));
    }

    return params_string;
}