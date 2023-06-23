#include "types_controllers.h"

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

void destroy_pcb(t_pcb *pcb)
{
    lista_instrucciones_destroy(pcb->instrucciones);
    registers_destroy(pcb->registers);
    segments_table_destroy(pcb->segments_table);
    temporal_destroy(pcb->tiempo_llegada_ready);
    temporal_destroy(pcb->tiempo_entrada_cpu);
    temporal_destroy(pcb->tiempo_salida_cpu);
    list_destroy(pcb->open_files_table);
    free(pcb);
}

void info_destroy(t_info *info) {
    free(info->data);
    free(info);
}

void info_write_destroy(t_info_write *info_write) {
    info_destroy(info_write->info);
    free(info_write);
}
