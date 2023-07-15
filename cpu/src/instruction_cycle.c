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
    t_instruccion *instruccionListaParaEjecutar = instruccion_duplicate(instruccionSiguiente);

    switch (instruccionListaParaEjecutar->identificador)
    {
    case I_SET:
        sleep(atoi(config->retardo_instruccion) / 1000); // Miliseconds -> Seconds
        break;
    case I_MOV_IN:
        char *dir_logMI = string_duplicate((char*)list_get(instruccionListaParaEjecutar->parametros, 1));
        seg_fault = chequear_seg_fault_movs(dir_logMI, contexto->segments, (char*)list_get(instruccionListaParaEjecutar->parametros, 0));
        if (!seg_fault){
            instruccionListaParaEjecutar = cambiar_dir_logica_a_fisica_movs(instruccionListaParaEjecutar, contexto->segments, 1);
        }
        list_add(instruccionListaParaEjecutar->parametros, dir_logMI);
        break;
    case I_MOV_OUT:
        char *dir_logMO = string_duplicate((char*)list_get(instruccionListaParaEjecutar->parametros, 0));
        seg_fault = chequear_seg_fault_movs(dir_logMO, contexto->segments, (char*)list_get(instruccionListaParaEjecutar->parametros, 1));
        if (!seg_fault){
            instruccionListaParaEjecutar = cambiar_dir_logica_a_fisica_movs(instruccionListaParaEjecutar, contexto->segments, 0);
        }
        list_add(instruccionListaParaEjecutar->parametros, dir_logMO);
        break;
    case I_F_READ:
        seg_fault = checkear_seg_fault(instruccionListaParaEjecutar, contexto->segments);
        instruccionListaParaEjecutar = cambiar_dir_logica_a_fisica_files(instruccionListaParaEjecutar, contexto->segments);
        break;
    case I_F_WRITE:
        seg_fault = checkear_seg_fault(instruccionListaParaEjecutar, contexto->segments);
        instruccionListaParaEjecutar = cambiar_dir_logica_a_fisica_files(instruccionListaParaEjecutar, contexto->segments);
        break;
    case I_CREATE_SEGMENT:
        seg_fault = chequear_seg_fault_segment_size(instruccionListaParaEjecutar);
    default:
        break;
    }

    return instruccionListaParaEjecutar;
}

t_pcontexto_desalojo *execute(t_instruccion *instruccionListaParaEjecutar, t_pcontexto *contexto)
{
    char *valor;
    switch (instruccionListaParaEjecutar->identificador)
    {
    case I_SET:
        SET((char*)list_get(instruccionListaParaEjecutar->parametros, 0), (char*)list_get(instruccionListaParaEjecutar->parametros, 1));
        break;
    case I_MOV_IN:
        valor = MOV_IN((char*)list_get(instruccionListaParaEjecutar->parametros, 0), (char*)list_get(instruccionListaParaEjecutar->parametros, 1), contexto->pid);
        if (valor != NULL){
            log_info(logger, "PID: %d - Accion: LEER - Segmento: %d - Direccion Fisica: %s - valor: %s",
                contexto->pid, 
                get_num_segmento((char *)list_get(instruccionListaParaEjecutar->parametros, 2)), 
                (char *)list_get(instruccionListaParaEjecutar->parametros, 1), 
                valor);
            free(valor);
        }
        else
            seg_fault = true;
        char *dir_logMI = list_remove(instruccionListaParaEjecutar->parametros, 2); // elimino el numero de segmento que agregue en decode
        free(dir_logMI);
        break;
    case I_MOV_OUT:
        valor = MOV_OUT((char*)list_get(instruccionListaParaEjecutar->parametros, 0), (char*)list_get(instruccionListaParaEjecutar->parametros, 1), contexto->pid);
        if (valor != NULL){
            log_info(logger, "PID: %d - Accion: ESCRIBIR - Segmento: %d - Direccion Fisica: %s - valor: %s", 
                contexto->pid, 
                get_num_segmento((char *)list_get(instruccionListaParaEjecutar->parametros, 2)), 
                (char*)list_get(instruccionListaParaEjecutar->parametros, 0), 
                valor);
            free(valor);
        } else 
            seg_fault = true;
        char *dir_logMO = list_remove(instruccionListaParaEjecutar->parametros, 2); // elimino el numero de segmento que agregue en decode
        free(dir_logMO);
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
    char *params_string = get_params_string(instruccionSiguiente);
    char *instruction_string = get_instruction_string(instruccionSiguiente->identificador);
    log_info(logger, "PID: %d - Ejecutando: %s - %s", contexto->pid, instruction_string, params_string);
    free(params_string);

    t_pcontexto_desalojo *contexto_desalojo;

    if(seg_fault) {
        seg_fault = false;
        int s = 0;
        int o = 0;
        int t = 0;
        
        switch (instruccionListaParaEjecutar->identificador)
        {
        case I_MOV_IN:
            s = get_num_segmento((char *)list_get(instruccionListaParaEjecutar->parametros, 2));
            o = get_desplazamiento_segmento((char*)list_get(instruccionListaParaEjecutar->parametros, 1));
            t = get_sizeof_register((char*)list_get(instruccionListaParaEjecutar->parametros, 0));
            char *dir_logMI = list_remove(instruccionListaParaEjecutar->parametros, 2); // elimino el numero de segmento que agregue en decode
            free(dir_logMI);
            break;
        case I_MOV_OUT:
            s = get_num_segmento((char *)list_get(instruccionListaParaEjecutar->parametros, 2));
            o = get_desplazamiento_segmento((char*)list_get(instruccionListaParaEjecutar->parametros, 0));
            t = get_sizeof_register((char*)list_get(instruccionListaParaEjecutar->parametros, 1));
            char *dir_logMO = list_remove(instruccionListaParaEjecutar->parametros, 2); // elimino el numero de segmento que agregue en decode
            free(dir_logMO);
            break;
        case I_F_READ:
        case I_F_WRITE:
            s = *((int*)list_get(instruccionListaParaEjecutar->parametros, 4));
            o = *((int*)list_get(instruccionListaParaEjecutar->parametros, 3));
            t = get_by_num_segmento(atoi((char*)list_get(instruccionListaParaEjecutar->parametros, 4)), contexto->segments)->size;
            list_remove(instruccionListaParaEjecutar->parametros, 4); // elimino el numero de segmento que agregue en decode
            list_remove(instruccionListaParaEjecutar->parametros, 3); // elimino el offset que agregue en decode
            break;
        case I_CREATE_SEGMENT:
            s = atoi((char*)list_get(instruccionListaParaEjecutar->parametros, 0));
            t = atoi((char*)list_get(instruccionListaParaEjecutar->parametros, 1));
            break;
        default:
            break;
        }
        
        log_error(logger, "PID: %d - Error SEG_FAULT - Segmento: %d - Offset: %d - Tamanio: %d", contexto->pid, s, o, t);
        if (instruccionListaParaEjecutar->identificador == I_CREATE_SEGMENT)
            contexto_desalojo = stop_exec(contexto, instruccionListaParaEjecutar, OUT_OF_MEMORY);
        else
            contexto_desalojo = stop_exec(contexto, instruccionListaParaEjecutar, SEGMENTATION_FAULT);
    } else {
        contexto_desalojo = execute(instruccionListaParaEjecutar, contexto);
        if (seg_fault)
        {
            seg_fault = false;
            log_error(logger, "PID: %d - Error SEG_FAULT - Modulo: MEMORIA", contexto->pid);
            contexto_desalojo = stop_exec(contexto, instruccionListaParaEjecutar, SEGMENTATION_FAULT);
        }
    }

    instruccion_destroy(instruccionListaParaEjecutar);

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
    char *params_string = string_new();
    if (instruction->cant_parametros == 0){
        string_append(&params_string, "Sin parametros");
    }else {
        for (int i = 0; i < instruction->cant_parametros; i++)
        {
            string_append(&params_string, list_get(instruction->parametros, i));
            if (i != instruction->cant_parametros - 1)
                string_append(&params_string, " ");
        }
    }
    return params_string;
}

// le decis en que index esta la dir logica y te crea una nueva instruccion igual pero con la dir fisica
t_instruccion *cambiar_dir_logica_a_fisica_movs(t_instruccion *instruccion, t_list *segments, int index_parametro) {
    char *direccion_fisica = get_direccion_fisica((char*)list_get(instruccion->parametros, index_parametro), segments);

    t_list *params = list_create();

    switch(index_parametro) {
        case 0:
            list_add(params, direccion_fisica);
            list_add(params, string_duplicate((char*)list_get(instruccion->parametros, 1)));
            break;
        case 1:
            list_add(params, string_duplicate((char*)list_get(instruccion->parametros, 0)));
            list_add(params, direccion_fisica);
            break;
        default:
            return NULL;
    }

    t_instruccion *instruccion_new = create_new_instruction(instruccion->identificador, params);

    instruccion_destroy(instruccion);

    return instruccion_new;
}

t_instruccion *cambiar_dir_logica_a_fisica_files(t_instruccion *instruccion, t_list *segments) {
    char *direccion_fisica = get_direccion_fisica((char*)list_get(instruccion->parametros, 1), segments);

    t_list *params = list_create();

    list_add(params, string_duplicate((char*)list_get(instruccion->parametros, 0)));
    list_add(params, direccion_fisica);
    list_add(params, string_duplicate((char*)list_get(instruccion->parametros, 2)));

    t_instruccion *instruccion_new = create_new_instruction(instruccion->identificador, params);

    instruccion_destroy(instruccion);

    return instruccion_new;
}

bool checkear_seg_fault(t_instruccion *instruction, t_list *segments) {
    int num_seg = get_num_segmento((char*)list_get(instruction->parametros, 1));
    t_segment *segment = get_by_num_segmento(num_seg, segments);

    int desplazamiento = get_desplazamiento_segmento(list_get(instruction->parametros, 1));
    int tamanio_a_leer_escribir = atoi((char*)list_get(instruction->parametros, 2));

    log_debug(logger_aux, "Chequeando SEG_FAULT FILE - Segmento: %d - Tamanio Segmento: %d - Desplazamiento: %d - Tamanio: %d", num_seg, segment->size, desplazamiento, tamanio_a_leer_escribir);
    bool res = (tamanio_a_leer_escribir + desplazamiento) > segment->size;

    if(res) {
        int offset = desplazamiento + tamanio_a_leer_escribir - segment->size;
        list_add(instruction->parametros, &offset);
        list_add(instruction->parametros, &num_seg);
    }

    return res;
}

bool chequear_seg_fault_segment_size(t_instruccion *instruction)
{
    int tam_seg = atoi((char*)list_get(instruction->parametros, 1));
    int tam_seg_max = atoi(config->tam_max_segmento);
    return tam_seg > tam_seg_max;
}

bool chequear_seg_fault_movs(char *dir_logica, t_list *segments, char *registro)
{
    int num_seg = get_num_segmento(dir_logica);
    t_segment *segment = get_by_num_segmento(num_seg, segments);
    int desplazamiento = get_desplazamiento_segmento(dir_logica);
    
    int tamanio_a_leer_escribir = get_sizeof_register(registro);

    log_debug(logger_aux, "Chequeando SEG_FAULT MOVS - Segmento: %d - Tamanio Segmento: %d - Desplazamiento: %d - Tamanio: %d", num_seg, segment->size, desplazamiento, tamanio_a_leer_escribir);

    bool res = (tamanio_a_leer_escribir + desplazamiento) > segment->size;

    return res;
}