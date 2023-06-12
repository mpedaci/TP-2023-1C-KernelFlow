#include "instruction_cycle.h"

t_instruccion *fetch(t_pcontexto *contexto)
{
    t_instruccion *instruccionSiguiente = list_get(contexto->instructions, contexto->program_counter);
    contexto->program_counter++;
    return instruccionSiguiente;
}

t_instruccion *decode(t_instruccion *instruccionSiguiente, t_pcontexto *contexto)
{
    // creando instruccion lista para ejecutar
    t_instruccion *instruccionListaParaEjecutar = new_instruction(instruccionSiguiente);

    switch (instruccionListaParaEjecutar->identificador)
    {
    case I_SET:
        sleep(atoi(config->retardo_instruccion) / 1000); // Miliseconds -> Seconds
        break;
    case I_MOV_IN:
        int num_segmento = get_num_segmento(list_get(instruccionListaParaEjecutar->parametros, 1));
        cambiar_dir_logica_a_fisica(instruccionListaParaEjecutar, contexto->segmentos, 1);
        list_add(instruccionListaParaEjecutar->parametros, num_segmento);
        break;
    case I_MOV_OUT:
        int num_segmento = get_num_segmento(list_get(instruccionListaParaEjecutar->parametros, 0));
        cambiar_dir_logica_a_fisica(instruccionListaParaEjecutar, contexto->segmentos, 0);
        list_add(instruccionListaParaEjecutar->parametros, num_segmento);
        break;
    case I_F_READ:
        cambiar_dir_logica_a_fisica(instruccionListaParaEjecutar, contexto->segmentos, 1);
        break;
    case I_F_WRITE:
        cambiar_dir_logica_a_fisica(instruccionListaParaEjecutar, contexto->segmentos, 1);
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
        char *valor = MOV_IN(list_get(instruccionListaParaEjecutar->parametros, 0), list_get(instruccionListaParaEjecutar->parametros, 1));
        log_info(logger, "PID: %d - Accion: LEER - Segmento: %d - Direccion Fisica: %s - valor: %s", contexto->pid, list_get(instruccionListaParaEjecutar->parametros, 2), list_get(instruccionListaParaEjecutar->parametros, 1), valor);
        list_remove(instruccionListaParaEjecutar->parametros, 2); // elimino el numero de segmento que agregue en decode
        break;
    case I_MOV_OUT:
        char *valor = MOV_OUT(list_get(instruccionListaParaEjecutar->parametros, 0), list_get(instruccionListaParaEjecutar->parametros, 1));
        log_info(logger, "PID: %d - Accion: ESCRIBIR - Segmento: %d - Direccion Fisica: %s - valor: %s", contexto->pid, list_get(instruccionListaParaEjecutar->parametros, 2), list_get(instruccionListaParaEjecutar->parametros, 0), valor);
        list_remove(instruccionListaParaEjecutar->parametros, 2); // elimino el numero de segmento que agregue en decode
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
        log_error(logger_aux, "No se pudo encontrar la instruccion :(\n");
        break;
    }
    return NULL;
}

t_pcontexto_desalojo *execute_instruction_cycle(t_pcontexto *contexto)
{
    t_instruccion *instruccionSiguiente = fetch(contexto);
    t_instruccion *instruccionListaParaEjecutar = decode(instruccionSiguiente, contexto);

    // loggeo la instruccion ejecutada
    // char *params_string = get_params_string(instruccionListaParaEjecutar);
    // char *instruction_string = get_instruction_string(instruccionListaParaEjecutar->identificador);
    // log_info(logger, "PID: %d - Ejecutando: %s - %s", contexto->pid, instruction_string, params_string);
    // free(params_string);

    t_pcontexto_desalojo *contexto_desalojo = execute(instruccionListaParaEjecutar, contexto);

    instruction_destroyer(instruccionListaParaEjecutar);

    return contexto_desalojo;
}

t_pcontexto_desalojo *execute_process(t_pcontexto *contexto)
{
    ejecutando = true;
    t_pcontexto_desalojo *contexto_desalojo;
    while (ejecutando)
        contexto_desalojo = execute_instruction_cycle(contexto);
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
        return "Sin parametros";
    char *params_string = string_new();
    for (int i = 0; i < instruction->cant_parametros; i++)
    {
        string_append(&params_string, list_get(instruction->parametros, i));
        if (i != instruction->cant_parametros - 1)
            string_append(&params_string, " ");
    }
    return params_string;
}

void cambiar_dir_logica_a_fisica(t_instruccion *instruccion, t_list *segmentos, int index_parametro) {
    char *direccion_fisica = get_direccion_fisica(list_get(instruccion->parametros, index_parametro), segmentos);
    list_remove(instruccion->parametros, index_parametro);
    list_add(instruccion->parametros, direccion_fisica);
    instruccion->p_length[index_parametro] = strlen(direccion_fisica) + 1;
}