#include "instruction_cycle.h"

t_instruccion* fetch(t_pcontexto* contexto) {
    t_instruccion* instruccionSiguiente = list_get(contexto->instructions, contexto->program_counter);
    contexto->program_counter++;
    return instruccionSiguiente;
}


t_instruccion* decode(t_instruccion* instruccionSiguiente) {
    t_instruccion* instruccionListaParaEjecutar;
    instruccionListaParaEjecutar->identificador = instruccionSiguiente->identificador;
    instruccionListaParaEjecutar->cant_parametros = instruccionSiguiente->cant_parametros;
    
    switch (instruccionListaParaEjecutar->identificador)
    {
    case I_SET:
        // AGREGAR RETARDO DE INSTRUCCION
        break;
    case I_MOV_OUT:
        // mmu traduce
        break;
    case I_F_READ:
        // mmu traduce
        break;
    case I_F_WRITE:
        // mmu traduce
        break;
    default:
        instruccionListaParaEjecutar->parametros = instruccionSiguiente->parametros;
        break;
    }

    return instruccionListaParaEjecutar;
}


void execute(t_instruccion* instruccionListaParaEjecutar) {
    switch (instruccionListaParaEjecutar->identificador)
    {
    case I_SET:
        char *registro = instruccionListaParaEjecutar->parametros[0];
        char *valor = instruccionListaParaEjecutar->parametros[1];
        SET(registro, valor);
        break;
    case I_MOV_IN:
        
        break;
    case I_MOV_OUT:
        
        break;
    case I_I_O:
        
        break;
    case I_F_OPEN:
        
        break;
    case I_F_CLOSE:
        
        break;
    case I_F_SEEK:
        
        break;
    case I_F_READ:
        
        break;
    case I_F_WRITE:
        
        break;
    case I_F_TRUNCATE:
        
        break;
    case I_WAIT:
        
        break;
    case I_SIGNAL:
        
        break;
    case I_CREATE_SEGMENT:
        
        break;
    case I_DELETE_SEGMENT:
        
        break;
    case I_YIELD:
        YIELD();
        break;
    case I_EXIT:
        EXIT();
        break;
    default:
        printf("No se pudo encontrar la instruccion :(");
        break;
    }
}

t_pcontexto* execute_instruction_cycle(t_pcontexto* contexto) {
    t_instruccion* instruccionSiguiente = fetch(contexto);
    t_instruccion* instruccionListaParaEjecutar = decode(instruccionSiguiente);
    execute(instruccionListaParaEjecutar);
    return contexto;
}

t_pcontexto* execute_process(t_pcontexto* contexto) {
    ejecutando = true;
    while(!ejecutando) {
        execute_instruction_cycle(contexto);
    }
    return contexto;
}