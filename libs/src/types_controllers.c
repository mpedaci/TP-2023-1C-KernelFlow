#include "types_controllers.h"

// CONSTRUCTORS

t_pid_status *pid_status_create(uint32_t pid, t_status_code status)
{
    t_pid_status *pid_status = malloc(sizeof(t_pid_status));
    pid_status->pid = pid;
    pid_status->status = status;
    return pid_status;
}

t_pcontexto *pcontexto_create(t_pcb *pcb)
{
    t_pcontexto *pcontexto = malloc(sizeof(t_pcontexto));
    pcontexto->pid = pcb->pid;
    pcontexto->program_counter = pcb->program_counter;
    pcontexto->registers = registers_create();
    registers_duplicate(pcb->registers, pcontexto->registers);
    pcontexto->instructions = lista_instrucciones_duplicate(pcb->instrucciones);
    pcontexto->segments = segment_list_duplicate(pcb->segments_table);
    return pcontexto;
}

t_registers *registers_create()
{
    t_registers *registers = malloc(sizeof(t_registers));
    registers->AX = malloc(4);
    registers->BX = malloc(4);
    registers->CX = malloc(4);
    registers->DX = malloc(4);
    registers->EAX = malloc(8);
    registers->EBX = malloc(8);
    registers->ECX = malloc(8);
    registers->EDX = malloc(8);
    registers->RAX = malloc(16);
    registers->RBX = malloc(16);
    registers->RCX = malloc(16);
    registers->RDX = malloc(16);
    char *zero = "0000000000000000";
    memcpy(registers->AX, zero, 4);
    memcpy(registers->BX, zero, 4);
    memcpy(registers->CX, zero, 4);
    memcpy(registers->DX, zero, 4);
    memcpy(registers->EAX, zero, 8);
    memcpy(registers->EBX, zero, 8);
    memcpy(registers->ECX, zero, 8);
    memcpy(registers->EDX, zero, 8);
    memcpy(registers->RAX, zero, 16);
    memcpy(registers->RBX, zero, 16);
    memcpy(registers->RCX, zero, 16);
    memcpy(registers->RDX, zero, 16);
    return registers;
}

// DUPLICATORS

void registers_duplicate(t_registers *from, t_registers *to)
{
    memcpy(to->AX, from->AX, 4);
    memcpy(to->BX, from->BX, 4);
    memcpy(to->CX, from->CX, 4);
    memcpy(to->DX, from->DX, 4);
    memcpy(to->EAX, from->EAX, 8);
    memcpy(to->EBX, from->EBX, 8);
    memcpy(to->ECX, from->ECX, 8);
    memcpy(to->EDX, from->EDX, 8);
    memcpy(to->RAX, from->RAX, 16);
    memcpy(to->RBX, from->RBX, 16);
    memcpy(to->RCX, from->RCX, 16);
    memcpy(to->RDX, from->RDX, 16);
}

t_list *lista_instrucciones_duplicate(t_list *instructions)
{
    t_list *new_list = list_create();
    for (int i = 0; i < list_size(instructions); i++)
    {
        t_instruccion *a_duplicar = list_get(instructions, i);
        t_instruccion *instruction = instruccion_duplicate(a_duplicar);
        list_add(new_list, instruction);
    }
    return new_list;
}

t_instruccion *instruccion_duplicate(t_instruccion *instruccion)
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

t_list *segment_list_duplicate(t_segments_table *segments_table)
{
    t_list *new_list = list_create();
    for (int i = 0; i < list_size(segments_table->segment_list); i++)
    {
        t_segment *segment = segment_duplicate(((t_segment *)list_get(segments_table->segment_list, i)));
        list_add(new_list, segment);
    }
    return new_list;
}

t_segment *segment_duplicate(t_segment *segment)
{
    t_segment *new_segment = malloc(sizeof(t_segment));
    new_segment->id = segment->id;
    new_segment->base_address = segment->base_address;
    new_segment->size = segment->size;
    return new_segment;
}

// DESTROYERS

void instruccion_destroy(t_instruccion *instruccion)
{
    list_destroy_and_destroy_elements(instruccion->parametros, free);
    free(instruccion);
}

void lista_instrucciones_destroy(t_list *lista_instrucciones)
{
    list_destroy_and_destroy_elements(lista_instrucciones, (void *)instruccion_destroy);
}

void pcontexto_destroy(t_pcontexto *contexto)
{
    lista_instrucciones_destroy(contexto->instructions);
    registers_destroy(contexto->registers);
    list_destroy_and_destroy_elements(contexto->segments, free);
    free(contexto);
}

void pcontexto_desalojo_destroy(t_pcontexto_desalojo *pcontexto)
{
    lista_instrucciones_destroy(pcontexto->instructions);
    registers_destroy(pcontexto->registers);
    instruccion_destroy(pcontexto->motivo_desalojo);
    free(pcontexto);
}

void registers_destroy(t_registers *registers)
{
    free(registers->AX);
    free(registers->BX);
    free(registers->CX);
    free(registers->DX);
    free(registers->EAX);
    free(registers->EBX);
    free(registers->ECX);
    free(registers->EDX);
    free(registers->RAX);
    free(registers->RBX);
    free(registers->RCX);
    free(registers->RDX);
    free(registers);
}

void segments_table_destroy(t_segments_table *s_table)
{
    list_destroy_and_destroy_elements(s_table->segment_list, free);
    free(s_table);
}

void pcb_destroy(t_pcb *pcb)
{
    lista_instrucciones_destroy(pcb->instrucciones);
    registers_destroy(pcb->registers);
    if (pcb->segments_table != NULL)
        segments_table_destroy(pcb->segments_table);
    temporal_destroy(pcb->tiempo_llegada_ready);
    temporal_destroy(pcb->tiempo_entrada_cpu);
    temporal_destroy(pcb->tiempo_salida_cpu);
    if (list_size(pcb->open_files_table) > 0)
        list_destroy_and_destroy_elements(pcb->open_files_table, free);
    else
        list_destroy(pcb->open_files_table);
    list_destroy(pcb->shared_resources);
    free(pcb);
}

void info_destroy(t_info *info)
{
    free(info->data);
    free(info);
}

void info_write_destroy(t_info_write *info_write)
{
    info_destroy(info_write->info);
    free(info_write);
}

void pid_status_destroy(t_pid_status *pid_status)
{
    free(pid_status);
}

void data_destroy(t_data *data)
{
    free(data->value);
    free(data);
}

// TO STRING

char *status_code_string(t_status_code sc)
{
    switch (sc)
    {
    case SEGMENTATION_FAULT:
        return "SEGMENTATION FAULT";
    case COMPACTATION_REQUIRED:
        return "COMPACTATION REQUIRED";
    case OUT_OF_MEMORY:
        return "OUT OF MEMORY";
    case FILE_OPEN:
        return "FILE OPEN";
    case FILE_CREATED:
        return "FILE CREATED";
    case FILE_READ:
        return "FILE READ";
    case FILE_TRUNCATED:
        return "FILE TRUNCATED";
    case FILE_WRITTEN:
        return "FILE WRITTEN";
    case SUCCESS:
        return "SUCCESS";
    case PROCESS_NEW:
        return "PROCESS NEW";
    case PROCESS_END:
        return "PROCESS END";
    case ERROR:
        return "ERROR";
    case INVALID_RESOURCE:
        return "INVALID RESOURCE";
    case MAX_SEG_QUANTITY_REACHED:
        return "MAX SEG QUANTITY REACHED";
    case PROCESS_ABORTED:
        return "PROCESS ABORTED";
    default:
        return "UNKNOWN";
    }
}