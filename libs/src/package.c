#include "package.h"

/* BUFFERS TIPOS DE DATOS -> SEND */

uint32_t espacio_de_array_parametros(t_instruccion *instruccion)
{
    uint32_t espacio = 0;
    for (int i = 0; i < 4; i++)
        espacio += instruccion->p_length[i];
    return espacio;
}

t_buffer *t_instruccion_create_buffer(t_instruccion *instruccion)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) +                         // identificador
                   sizeof(uint32_t) +                         // cant_parametros
                   espacio_de_array_parametros(instruccion) + // tam parametros
                   sizeof(uint32_t) * 4;                      // 4 p_length
    void *stream = malloc(buffer->size);
    int offset = 0;
    memcpy(stream + offset, &instruccion->identificador, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->cant_parametros, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    for (size_t i = 0; i < 4; i++)
    {
        memcpy(stream + offset, &instruccion->p_length[i], sizeof(uint32_t));
        offset += sizeof(uint32_t);
    }
    for (int i = 0; i < instruccion->cant_parametros; i++)
    {
        memcpy(stream + offset, list_get(instruccion->parametros, i), instruccion->p_length[i]);
        offset += instruccion->p_length[i];
    }
    buffer->stream = stream;
    return buffer;
}

t_buffer *t_lista_instrucciones_create_buffer(t_list *lista_instrucciones)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    uint32_t size_total = 0;
    for (int i = 0; i < list_size(lista_instrucciones); i++)
    {
        t_instruccion *instruccion = list_get(lista_instrucciones, i);
        t_buffer *buffer_instruccion = t_instruccion_create_buffer(instruccion); // 10 - 20 - 4
        size_total += buffer_instruccion->size;
        free(buffer_instruccion->stream);
        free(buffer_instruccion);
    }
    // creo el stream y copio los datos de cada buffer
    void *stream = malloc(size_total);
    buffer->size = size_total;
    uint32_t offset = 0;
    for (int i = 0; i < list_size(lista_instrucciones); i++)
    {
        t_buffer *buffer_instruccion = t_instruccion_create_buffer(list_get(lista_instrucciones, i));
        uint32_t size = buffer_instruccion->size;
        void *stream_instruccion = buffer_instruccion->stream;
        memcpy(stream + offset, stream_instruccion, size);
        offset += size;
        free(buffer_instruccion->stream);
        free(buffer_instruccion);
    }
    buffer->stream = stream;
    return buffer;
}

t_buffer *t_pcontexto_create_buffer(t_pcontexto *pcontexto)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    t_buffer *buffer_instrucciones = t_lista_instrucciones_create_buffer(pcontexto->instructions);
    
    t_segments_table *st = malloc(sizeof(t_segments_table));
    st->pid = pcontexto->pid;
    st->segment_list = pcontexto->segments;
    t_buffer *buffer_t_segmentos = t_segments_table_create_buffer(st);
    free(st);

    buffer->size = sizeof(uint32_t) * 4 +       // pid + program_counter + size lista instrucciones + size tabla segmentos
                   buffer_instrucciones->size + // lista instrucciones
                   buffer_t_segmentos->size +   // tabla de segmentos
                   4 * 4 +                      // registers->AX
                   8 * 4 +                      // registers->EAX
                   16 * 4;                      // registers->RAX

    void *stream = malloc(buffer->size);

    int offset = 0;

    memcpy(stream, &pcontexto->pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &(pcontexto->program_counter), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Registros

    memcpy(stream + offset, pcontexto->registers->AX, 4);
    offset += 4;
    memcpy(stream + offset, pcontexto->registers->BX, 4);
    offset += 4;
    memcpy(stream + offset, pcontexto->registers->CX, 4);
    offset += 4;
    memcpy(stream + offset, pcontexto->registers->DX, 4);
    offset += 4;

    memcpy(stream + offset, pcontexto->registers->EAX, 8);
    offset += 8;
    memcpy(stream + offset, pcontexto->registers->EBX, 8);
    offset += 8;
    memcpy(stream + offset, pcontexto->registers->ECX, 8);
    offset += 8;
    memcpy(stream + offset, pcontexto->registers->EDX, 8);
    offset += 8;

    memcpy(stream + offset, pcontexto->registers->RAX, 16);
    offset += 16;
    memcpy(stream + offset, pcontexto->registers->RBX, 16);
    offset += 16;
    memcpy(stream + offset, pcontexto->registers->RCX, 16);
    offset += 16;
    memcpy(stream + offset, pcontexto->registers->RDX, 16);
    offset += 16;

    // Instrucciones

    memcpy(stream + offset, &buffer_instrucciones->size, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &buffer_t_segmentos->size, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, buffer_instrucciones->stream, buffer_instrucciones->size);
    offset += buffer_instrucciones->size;

    free(buffer_instrucciones->stream);
    free(buffer_instrucciones);

    memcpy(stream + offset, buffer_t_segmentos->stream, buffer_t_segmentos->size);
    offset += buffer_t_segmentos->size;

    free(buffer_t_segmentos->stream);
    free(buffer_t_segmentos);

    buffer->stream = stream;
    return buffer;
}

t_buffer *t_pcontexto_desalojo_create_buffer(t_pcontexto_desalojo *pcontexto)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    t_buffer *buffer_instrucciones = t_lista_instrucciones_create_buffer(pcontexto->instructions);
    t_buffer *buffer_instruccion_desalojo = t_instruccion_create_buffer(pcontexto->motivo_desalojo);

    buffer->size = sizeof(uint32_t) * 5 +              // pid + program_counter + status_code + size lista instrucciones + size instruccion desalojo
                   buffer_instrucciones->size +        // lista instrucciones
                   buffer_instruccion_desalojo->size + // instruccion desalojo
                   4 * 4 +                             // registers->AX
                   8 * 4 +                             // registers->EAX
                   16 * 4;                             // registers->RAX

    void *stream = malloc(buffer->size);

    int offset = 0;

    memcpy(stream, &pcontexto->pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &(pcontexto->program_counter), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &(pcontexto->status_code), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Registros

    memcpy(stream + offset, pcontexto->registers->AX, 4);
    offset += 4;
    memcpy(stream + offset, pcontexto->registers->BX, 4);
    offset += 4;
    memcpy(stream + offset, pcontexto->registers->CX, 4);
    offset += 4;
    memcpy(stream + offset, pcontexto->registers->DX, 4);
    offset += 4;

    memcpy(stream + offset, pcontexto->registers->EAX, 8);
    offset += 8;
    memcpy(stream + offset, pcontexto->registers->EBX, 8);
    offset += 8;
    memcpy(stream + offset, pcontexto->registers->ECX, 8);
    offset += 8;
    memcpy(stream + offset, pcontexto->registers->EDX, 8);
    offset += 8;

    memcpy(stream + offset, pcontexto->registers->RAX, 16);
    offset += 16;
    memcpy(stream + offset, pcontexto->registers->RBX, 16);
    offset += 16;
    memcpy(stream + offset, pcontexto->registers->RCX, 16);
    offset += 16;
    memcpy(stream + offset, pcontexto->registers->RDX, 16);
    offset += 16;

    // instruccion desalojo

    memcpy(stream + offset, &buffer_instruccion_desalojo->size, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, buffer_instruccion_desalojo->stream, buffer_instruccion_desalojo->size);
    offset += buffer_instruccion_desalojo->size;

    free(buffer_instruccion_desalojo->stream);
    free(buffer_instruccion_desalojo);

    // Instrucciones

    memcpy(stream + offset, &buffer_instrucciones->size, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, buffer_instrucciones->stream, buffer_instrucciones->size);
    offset += buffer_instrucciones->size;

    free(buffer_instrucciones->stream);
    free(buffer_instrucciones);

    buffer->stream = stream;
    return buffer;
}

t_buffer *t_data_create_buffer(t_data *data)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t) * 1 + strlen(data->value) + 1;

    void *stream = malloc(buffer->size);

    int offset = 0;

    memcpy(stream + offset, &data->value_length, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, data->value, data->value_length + 1);

    buffer->stream = stream;
    return buffer;
}

t_buffer *t_address_create_buffer(t_address address)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) * 1;
    void *stream = malloc(buffer->size);
    memcpy(stream, &address, sizeof(uint32_t));
    buffer->stream = stream;
    return buffer;
}

t_buffer *t_open_files_create_buffer(t_open_files *open_files)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    // POR IMPLEMENTAR
    return buffer;
}

t_buffer *null_buffer()
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = 1;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream, "", buffer->size);
    return buffer;
}

t_buffer *t_segment_create_buffer(t_segment *segment)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t) * 3; // id + size + base_address

    void *stream = malloc(buffer->size);

    int offset = 0;

    memcpy(stream + offset, &segment->id, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &segment->size, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &segment->base_address, sizeof(uint32_t));

    buffer->stream = stream;
    return buffer;
}

t_buffer *t_segments_table_create_buffer(t_segments_table *segments_table)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    uint32_t cant_segmentos = list_size(segments_table->segment_list);
    buffer->size = sizeof(uint32_t) * 2 + // PID + cant segmentos
                   cant_segmentos * sizeof(uint32_t) * 3; // size de cada segmento (id + size + base_address)

    void *stream = malloc(buffer->size);
    uint32_t offset = 0;

    memcpy(stream + offset, &segments_table->pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &cant_segmentos, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    for (int i = 0; i < cant_segmentos; i++)
    {
        t_segment *segment = list_get(segments_table->segment_list, i);
        memcpy(stream + offset, &segment->id, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(stream + offset, &segment->size, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(stream + offset, &segment->base_address, sizeof(uint32_t));
        offset += sizeof(uint32_t);
    }
    buffer->stream = stream;
    return buffer;
}

t_buffer *t_lista_t_segments_create_buffer(t_list *lt_segments)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    uint32_t size_total = 0;
    for (int i = 0; i < list_size(lt_segments); i++)
    {
        t_buffer *buffer_tsegment = t_segments_table_create_buffer(list_get(lt_segments, i));
        size_total += buffer_tsegment->size;
        free(buffer_tsegment->stream);
        free(buffer_tsegment);
    }

    buffer->size = sizeof(uint32_t) +                           // Cantidad de Listas
                   sizeof(uint32_t) * list_size(lt_segments) +  // Size de cada Lista
                   size_total;                                  // Size de buffer
    void *stream = malloc(buffer->size);
    uint32_t offset = 0;
    uint32_t elements = list_size(lt_segments);
    memcpy(stream + offset, &elements, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    for (int i = 0; i < list_size(lt_segments); i++)
    {
        t_buffer *buffer_tsegment = t_segments_table_create_buffer(list_get(lt_segments, i));
        memcpy(stream + offset, &buffer_tsegment->size, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        free(buffer_tsegment->stream);
        free(buffer_tsegment);
    }

    for (int i = 0; i < list_size(lt_segments); i++)
    {
        t_buffer *buffer_tsegment = t_segments_table_create_buffer(list_get(lt_segments, i));
        memcpy(stream + offset, buffer_tsegment->stream, buffer_tsegment->size);
        offset += buffer_tsegment->size;
        free(buffer_tsegment->stream);
        free(buffer_tsegment);
    }

    buffer->stream = stream;
    return buffer;
}

t_buffer *t_status_code_create_buffer(t_status_code status_code)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) * 1;
    void *stream = malloc(buffer->size);
    memcpy(stream, &status_code, sizeof(uint32_t));
    buffer->stream = stream;
    return buffer;
};

t_buffer *t_pid_instruccion_create_buffer(t_pid_instruccion *pid_instruccion)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    t_buffer *buffer_instruccion = t_instruccion_create_buffer(pid_instruccion->instruccion);
    buffer->size = sizeof(uint32_t) * 2 +    // pid + size instruccion
                   buffer_instruccion->size; // instruccion
    void *stream = malloc(buffer->size);
    uint32_t offset = 0;
    memcpy(stream + offset, &(pid_instruccion->pid), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(buffer_instruccion->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, buffer_instruccion->stream, buffer_instruccion->size);
    free(buffer_instruccion->stream);
    free(buffer_instruccion);
    buffer->stream = stream;
    return buffer;
};

t_buffer *t_pid_status_create_buffer(t_pid_status *pid_status)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) * 2; // pid + status_code
    void *stream = malloc(buffer->size);
    uint32_t offset = 0;
    memcpy(stream + offset, &(pid_status->pid), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pid_status->status), sizeof(uint32_t));
    buffer->stream = stream;
    return buffer;
}

t_buffer *t_info_write_create_buffer(t_info_write *info_write) {
    t_buffer *buffer = malloc(sizeof(t_buffer));
    t_buffer *buffer_info = t_info_create_buffer(info_write->info);
    buffer->size = sizeof(uint32_t) * 2 + buffer_info->size; // 3er int es por el tam del buffer_info
    void *stream = malloc(buffer->size);
    uint32_t offset = 0;

    memcpy(stream + offset, &(info_write->base_address), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(buffer_info->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, buffer_info->stream, buffer_info->size);

    free(buffer_info->stream);
    free(buffer_info);
    
    buffer->stream = stream;
    return buffer;
}

t_buffer *t_info_read_create_buffer(t_info_read *info_read) {
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) * 2;
    void *stream = malloc(buffer->size);
    uint32_t offset = 0;

    memcpy(stream + offset, &(info_read->base_address), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(info_read->size), sizeof(uint32_t));
    buffer->stream = stream;
    return buffer;
}

t_buffer *t_info_create_buffer(t_info *info) {
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) + info->size;
    void *stream = malloc(buffer->size);
    uint32_t offset = 0;
    memcpy(stream + offset, &(info->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, info->data, info->size);
    buffer->stream = stream;
    return buffer;
}

/* BUFFERS TIPOS DE DATOS -> RECV */

t_instruccion *t_instruccion_create_from_buffer(t_buffer *buffer, uint32_t *offset)
{
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    instruccion->parametros = list_create();
    uint32_t offset_aux = 0;
    void *stream = buffer->stream;
    stream += (*offset);
    memcpy(&(instruccion->identificador), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->cant_parametros), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    offset_aux += sizeof(uint32_t) * 2;
    for (size_t i = 0; i < 4; i++)
    {
        memcpy(&(instruccion->p_length[i]), stream, sizeof(uint32_t));
        stream += sizeof(uint32_t);
    }
    offset_aux += sizeof(uint32_t) * 4;
    for (int i = 0; i < instruccion->cant_parametros; i++)
    {
        char *param = malloc(instruccion->p_length[i]);
        memcpy(param, stream, instruccion->p_length[i]);
        stream += instruccion->p_length[i];
        list_add(instruccion->parametros, param);
        offset_aux += instruccion->p_length[i];
    }
    *offset += offset_aux;
    return instruccion;
}

t_list *t_lista_instrucciones_create_from_buffer(t_buffer *buffer)
{
    t_list *lista_instrucciones = list_create();
    uint32_t offset = 0;
    while (offset < buffer->size)
    {
        t_instruccion *instruccion = t_instruccion_create_from_buffer(buffer, &offset);
        list_add(lista_instrucciones, instruccion);
    }
    return lista_instrucciones;
}

t_pcontexto *t_pcontexto_create_from_buffer(t_buffer *buffer)
{
    t_pcontexto *pcontexto = malloc(sizeof(t_pcontexto));

    pcontexto->registers = malloc(sizeof(t_registers));
    pcontexto->registers->AX = malloc(4);
    pcontexto->registers->BX = malloc(4);
    pcontexto->registers->CX = malloc(4);
    pcontexto->registers->DX = malloc(4);
    pcontexto->registers->EAX = malloc(8);
    pcontexto->registers->EBX = malloc(8);
    pcontexto->registers->ECX = malloc(8);
    pcontexto->registers->EDX = malloc(8);
    pcontexto->registers->RAX = malloc(16);
    pcontexto->registers->RBX = malloc(16);
    pcontexto->registers->RCX = malloc(16);
    pcontexto->registers->RDX = malloc(16);

    void *stream = buffer->stream;

    memcpy(&(pcontexto->pid), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    memcpy(&(pcontexto->program_counter), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    uint32_t offset = 4;
    memcpy(pcontexto->registers->AX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->BX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->CX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->DX, stream, offset);
    stream += offset;

    offset = 8;
    memcpy(pcontexto->registers->EAX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->EBX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->ECX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->EDX, stream, offset);
    stream += offset;

    offset = 16;
    memcpy(pcontexto->registers->RAX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->RBX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->RCX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->RDX, stream, offset);
    stream += offset;

    t_buffer *buffer_instructions = malloc(sizeof(t_buffer));
    t_buffer *buffer_t_segmentos = malloc(sizeof(t_buffer));

    memcpy(&(buffer_instructions->size), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    memcpy(&(buffer_t_segmentos->size), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    buffer_instructions->stream = malloc(buffer_instructions->size);
    memcpy(buffer_instructions->stream, stream, buffer_instructions->size);
    stream += buffer_instructions->size;

    pcontexto->instructions = t_lista_instrucciones_create_from_buffer(buffer_instructions);

    free(buffer_instructions->stream);
    free(buffer_instructions);

    buffer_t_segmentos->stream = malloc(buffer_t_segmentos->size);
    memcpy(buffer_t_segmentos->stream, stream, buffer_t_segmentos->size);
    stream += buffer_t_segmentos->size;

    uint32_t offset_aux = 0;

    t_segments_table *st = t_segments_table_create_from_buffer(buffer_t_segmentos, &offset_aux);

    free(buffer_t_segmentos->stream);
    free(buffer_t_segmentos);

    pcontexto->segments = st->segment_list;

    free(st);

    return pcontexto;
}

t_pcontexto_desalojo *t_pcontexto_desalojo_create_from_buffer(t_buffer *buffer)
{
    t_pcontexto_desalojo *pcontexto = malloc(sizeof(t_pcontexto_desalojo));

    pcontexto->registers = malloc(sizeof(t_registers));
    pcontexto->registers->AX = malloc(4);
    pcontexto->registers->BX = malloc(4);
    pcontexto->registers->CX = malloc(4);
    pcontexto->registers->DX = malloc(4);
    pcontexto->registers->EAX = malloc(8);
    pcontexto->registers->EBX = malloc(8);
    pcontexto->registers->ECX = malloc(8);
    pcontexto->registers->EDX = malloc(8);
    pcontexto->registers->RAX = malloc(16);
    pcontexto->registers->RBX = malloc(16);
    pcontexto->registers->RCX = malloc(16);
    pcontexto->registers->RDX = malloc(16);

    void *stream = buffer->stream;

    memcpy(&(pcontexto->pid), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    memcpy(&(pcontexto->program_counter), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    memcpy(&(pcontexto->status_code), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    // Registros

    uint32_t offset = 4;
    memcpy(pcontexto->registers->AX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->BX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->CX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->DX, stream, offset);
    stream += offset;

    offset = 8;
    memcpy(pcontexto->registers->EAX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->EBX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->ECX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->EDX, stream, offset);
    stream += offset;

    offset = 16;
    memcpy(pcontexto->registers->RAX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->RBX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->RCX, stream, offset);
    stream += offset;
    memcpy(pcontexto->registers->RDX, stream, offset);
    stream += offset;

    // instruccion desalojo
    t_buffer *buffer_instruccion = malloc(sizeof(t_buffer));

    memcpy(&(buffer_instruccion->size), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    buffer_instruccion->stream = malloc(buffer_instruccion->size);
    memcpy(buffer_instruccion->stream, stream, buffer_instruccion->size);
    stream += buffer_instruccion->size;

    uint32_t offset_aux = 0;

    pcontexto->motivo_desalojo = t_instruccion_create_from_buffer(buffer_instruccion, &offset_aux);

    free(buffer_instruccion->stream);
    free(buffer_instruccion);

    // Instrucciones

    t_buffer *buffer_instructions = malloc(sizeof(t_buffer));

    memcpy(&(buffer_instructions->size), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    buffer_instructions->stream = malloc(buffer_instructions->size);
    memcpy(buffer_instructions->stream, stream, buffer_instructions->size);
    stream += buffer_instructions->size;

    pcontexto->instructions = t_lista_instrucciones_create_from_buffer(buffer_instructions);

    free(buffer_instructions->stream);
    free(buffer_instructions);

    return pcontexto;
}

t_data *t_data_create_from_buffer(t_buffer *buffer)
{
    t_data *data = malloc(sizeof(t_data));
    void *stream = buffer->stream;

    memcpy(&(data->value_length), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    data->value = malloc(data->value_length);
    memcpy(data->value, stream, data->value_length);

    return data;
}

t_address t_address_create_from_buffer(t_buffer *buffer)
{
    t_address address;
    void *stream = buffer->stream;

    memcpy(&address, stream, sizeof(uint32_t));

    return address;
}

t_open_files *t_open_files_create_from_buffer(t_buffer *buffer)
{
    t_open_files *open_files = malloc(sizeof(t_open_files));
    // POR IMPLEMENTAR
    return open_files;
}

t_segment *t_segment_create_from_buffer(t_buffer *buffer, uint32_t *offset)
{
    t_segment *segment = malloc(sizeof(t_segment));
    void *stream = buffer->stream;
    stream += (*offset);
    memcpy(&(segment->id), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(segment->size), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(segment->base_address), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    *offset += sizeof(uint32_t) * 3;
    return segment;
}

t_segments_table *t_segments_table_create_from_buffer(t_buffer *buffer, uint32_t *offset)
{
    t_segments_table *segments_table = malloc(sizeof(t_segments_table));
    segments_table->segment_list = list_create();

    uint32_t offset_aux = 0;
    void *stream = buffer->stream;
    stream += (*offset);

    memcpy(&(segments_table->pid), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    offset_aux += sizeof(uint32_t);

    uint32_t cant_segmentos = 0;
    memcpy(&(cant_segmentos), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    offset_aux += sizeof(uint32_t);

    for (int i = 0; i < cant_segmentos; i++)
    {
        t_segment *segment = malloc(sizeof(t_segment));
        memcpy(&(segment->id), stream, sizeof(uint32_t));
        stream += sizeof(uint32_t);
        offset_aux += sizeof(uint32_t);
        memcpy(&(segment->size), stream, sizeof(uint32_t));
        stream += sizeof(uint32_t);
        offset_aux += sizeof(uint32_t);
        memcpy(&(segment->base_address), stream, sizeof(uint32_t));
        stream += sizeof(uint32_t);
        offset_aux += sizeof(uint32_t);
        list_add(segments_table->segment_list, segment);
    }

    *offset += offset_aux;
    return segments_table;
}

t_list *t_lista_t_segments_create_from_buffer(t_buffer *buffer)
{
    t_list *lista_t_segments = list_create();
    uint32_t cant_list = 0;
    uint32_t offset = 0;

    memcpy(&(cant_list), buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    uint32_t size_arr[cant_list];

    for (int i = 0; i < cant_list; i++)
    {
        memcpy(&(size_arr[i]), buffer->stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
    }

    for (int i = 0; i < cant_list; i++)
    {
        uint32_t offset_aux = 0;

        t_buffer *buffer_tsegment = malloc(sizeof(t_buffer));
        buffer_tsegment->size = size_arr[i];
        buffer_tsegment->stream = malloc(buffer_tsegment->size);

        memcpy(buffer_tsegment->stream, buffer->stream + offset, buffer_tsegment->size);
        offset += buffer_tsegment->size;

        t_segments_table *tsegment = t_segments_table_create_from_buffer(buffer_tsegment, &offset_aux);
        
        list_add(lista_t_segments, tsegment);

        free(buffer_tsegment->stream);
        free(buffer_tsegment);
    }

    return lista_t_segments;
};

t_status_code t_status_code_create_from_buffer(t_buffer *buffer)
{
    t_status_code status_code;
    memcpy(&(status_code), buffer->stream, sizeof(uint32_t));
    return status_code;
};

t_pid_instruccion *t_pid_instruccion_create_from_buffer(t_buffer *buffer)
{
    t_pid_instruccion *pid_instruccion = malloc(sizeof(t_pid_instruccion));
    void *stream = buffer->stream;
    memcpy(&(pid_instruccion->pid), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    t_buffer *buffer_instruccion = malloc(sizeof(t_buffer));
    memcpy(&(buffer_instruccion->size), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    buffer_instruccion->stream = malloc(buffer_instruccion->size);
    memcpy(buffer_instruccion->stream, stream, buffer_instruccion->size);
    stream += buffer_instruccion->size;

    uint32_t offset_aux = 0;
    pid_instruccion->instruccion = t_instruccion_create_from_buffer(buffer_instruccion, &offset_aux);
    free(buffer_instruccion->stream);
    free(buffer_instruccion);

    return pid_instruccion;
};

t_pid_status *t_pid_status_create_from_buffer(t_buffer *buffer)
{
    t_pid_status *pid_status = malloc(sizeof(t_pid_status));
    void *stream = buffer->stream;
    memcpy(&(pid_status->pid), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pid_status->status), stream, sizeof(uint32_t));
    return pid_status;
}

t_info_write *t_info_write_create_from_buffer(t_buffer *buffer) {
    t_info_write *info_write = malloc(sizeof(t_info_write));
    void *stream = buffer->stream;

    memcpy(&(info_write->base_address), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    // t_info
    t_buffer *buffer_info = malloc(sizeof(t_buffer));
    memcpy(&(buffer_info->size), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    buffer_info->stream = malloc(buffer_info->size);
    memcpy(buffer_info->stream, stream, buffer_info->size);

    info_write->info = t_info_create_from_buffer(buffer_info);

    free(buffer_info->stream);
    free(buffer_info);

    return info_write;
}

t_info_read *t_info_read_create_from_buffer(t_buffer *buffer) {
    t_info_read *info_read = malloc(sizeof(t_info_read));
    void *stream = buffer->stream;

    memcpy(&(info_read->base_address), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(info_read->size), stream, sizeof(uint32_t));
    return info_read;
}

t_info *t_info_create_from_buffer(t_buffer *buffer) {
    t_info *info = malloc(sizeof(t_info));
    void *stream = buffer->stream;
    memcpy(&(info->size), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    info->data = malloc(info->size);
    memcpy(info->data, stream, info->size);
    return info;
}

/* PAQUETES */

t_package *package_create(t_buffer *buffer, int operation_code)
{
    t_package *paquete = malloc(sizeof(t_package));
    paquete->operation_code = operation_code;
    if (buffer == NULL)
        paquete->buffer = null_buffer();
    else
        paquete->buffer = buffer;
    return paquete;
}

uint32_t package_get_size(t_package *paquete)
{
    return paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t);
}

bool package_send(int socket, t_package *paquete, t_log *logger)
{
    // Armamos el stream a enviar
    // Buffer Size + Size of Operation Code Var + Size of Buffer Size Var
    void *a_enviar = malloc(paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
    int offset = 0;
    memcpy(a_enviar + offset, &(paquete->operation_code), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
    if (send(socket, a_enviar, paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t), MSG_NOSIGNAL) == -1)
    {
        log_error(logger, "Error al enviar el paquete");
        free(a_enviar);
        return false;
    }
    free(a_enviar);
    return true;
}

t_package *package_recv(int socket, t_log *logger)
{
    t_package *paquete = malloc(sizeof(t_package));
    paquete->buffer = malloc(sizeof(t_buffer));

    // Primero recibimos el codigo de operacion
    recv(socket, &(paquete->operation_code), sizeof(uint8_t), 0);
    log_info(logger, "Recibido codigo de operacion: %d", paquete->operation_code);
    // Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
    recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);

    return paquete;
}

void package_destroy(t_package *paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}
