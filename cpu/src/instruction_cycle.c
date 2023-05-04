#include "instruction_cycle.h"

t_instruccion* fetch(t_pcontexto* contexto) {
    t_instruccion* instruccionSiguiente = list_get(contexto->instructions, contexto->program_counter);
    contexto->program_counter++;
    return instruccionSiguiente;
}


t_instruccion* decode(t_instruccion* instruccionSiguiente) {
    // creando instruccion lista para ejecutar
    t_instruccion* instruccionListaParaEjecutar = copy_instruction(instruccionSiguiente);


    // CAMBIAR LOS PARAMETROS EN LOS LUGARES QUE SON NECESARIOS (EJ MOV_OUT)
    // ASI COMO ESTA "instruccionListaParaEjecutar" TIENE LOS MISMOS PARAMETROS QUE LA "instruccionSiguiente"
    // MMU

    switch (instruccionListaParaEjecutar->identificador)
    {
    case I_SET:
        // AGREGAR RETARDO DE INSTRUCCION TODO
        break;
    case I_MOV_OUT:
        // mmu traduce
        // params = params traducidos por mmu
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


t_pcontexto_desalojo *execute(t_instruccion* instruccionListaParaEjecutar, t_pcontexto *contexto) {
    switch (instruccionListaParaEjecutar->identificador)
    {
    case I_SET:
        char *registro = instruccionListaParaEjecutar->parametros[0];
        char *valor = instruccionListaParaEjecutar->parametros[1];
        instruction_destroyer(instruccionListaParaEjecutar);
        SET(registro, valor);
        break;
    case I_MOV_IN:
        instruction_destroyer(instruccionListaParaEjecutar);
        break;
    case I_MOV_OUT:
        instruction_destroyer(instruccionListaParaEjecutar);
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

t_pcontexto_desalojo *execute_instruction_cycle(t_pcontexto* contexto) {
    t_instruccion* instruccionSiguiente = fetch(contexto);
    t_instruccion* instruccionListaParaEjecutar = decode(instruccionSiguiente);
    t_pcontexto_desalojo* contexto_desalojo = execute(instruccionListaParaEjecutar, contexto);
}

t_pcontexto_desalojo *execute_process(t_pcontexto* contexto) {
    ejecutando = true;
    while(!ejecutando) {
        execute_instruction_cycle(contexto);
    }
}