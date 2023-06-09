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

    buffer->size = sizeof(uint32_t) * 3 +       // pid + program_counter + size lista instrucciones
                   buffer_instrucciones->size + // lista instrucciones
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

    memcpy(stream + offset, buffer_instrucciones->stream, buffer_instrucciones->size);
    offset += buffer_instrucciones->size;

    free(buffer_instrucciones->stream);
    free(buffer_instrucciones);

    buffer->stream = stream;
    return buffer;
}

t_buffer *t_pcontexto_desalojo_create_buffer(t_pcontexto_desalojo *pcontexto)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    t_buffer *buffer_instrucciones = t_lista_instrucciones_create_buffer(pcontexto->instructions);
    t_buffer *buffer_instruccion_desalojo = t_instruccion_create_buffer(pcontexto->motivo_desalojo);

    buffer->size = sizeof(uint32_t) * 4 +              // pid + program_counter + size lista instrucciones + size instruccion desalojo
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
    buffer->size = 0;
    buffer->stream = NULL;
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
    return buffer;
}

t_buffer *t_lista_t_segments_create_buffer(t_list *lt_segments)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
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
    buffer->size = sizeof(uint32_t) +        // pid
                   buffer_instruccion->size; // instruccion
    void *stream = malloc(buffer->size);
    uint32_t offset = 0;
    memcpy(stream + offset, &pid_instruccion->pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, buffer_instruccion->stream, buffer_instruccion->size);
    free(buffer_instruccion->stream);
    free(buffer_instruccion);
    return buffer;
};

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

    *offset += sizeof(uint32_t) * 3;
    return segment;
}

t_segments_table *t_segments_table_create_from_buffer(t_buffer *buffer, uint32_t *offset)
{
    /* t_list *segments_table = list_create();
    uint32_t offset = 0;
    while (offset <= buffer->size)
    {
        t_segment *segment = t_segment_create_from_buffer(buffer, &offset);
        list_add(segments_table, segment);
    } */
    // PARA BORRAR
    t_segments_table *segments_table = malloc(sizeof(t_segments_table));
    return segments_table;
}

t_list *t_lista_t_segments_create_from_buffer(t_buffer *buffer)
{
    t_list *lista_t_segments = list_create();
    uint32_t offset = 0;
    while (offset <= buffer->size)
    {
        t_segment *segment = t_segment_create_from_buffer(buffer, &offset);
        list_add(lista_t_segments, segment);
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
    pid_instruccion->instruccion = t_instruccion_create_from_buffer(buffer, &offset_aux);
    
    free(buffer_instruccion->stream);
    free(buffer_instruccion);

    return pid_instruccion;
};

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
    if (send(socket, a_enviar, paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0) == -1)
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
