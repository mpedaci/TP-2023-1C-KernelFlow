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

// Memoria - Estructuras.c
/* void destroy_all_segments(){
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *segments_table = list_get(all_segments_tables, i);
        for (int j = 1; j < list_size(segments_table->segment_list); j++)
        {
            t_segment *segment = list_get(segments_table->segment_list, j);
            free(segment);
        }
    }
}

void destroy_all_segments_tables(){
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *segments_table = list_get(all_segments_tables, i);
        list_destroy(segments_table->segment_list);
        free(segments_table);
    }
}
 */

// Memoria - Segmentos.c
// void delete_segment(int pid, t_segment *segment);
// void delete_segments_table(t_segments_table *segments_table);