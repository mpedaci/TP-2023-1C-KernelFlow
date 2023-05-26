#include "package.h"

/* BUFFERS TIPOS DE DATOS -> SEND */

uint32_t espacio_de_array_parametros(t_instruccion *instruccion)
{
    uint32_t espacio = 0;
    for (int i = 0; i < instruccion->cant_parametros; i++)
        espacio += strlen(instruccion->parametros[i]) + 1;
    return espacio;
}

t_buffer *t_instruccion_create_buffer(t_instruccion *instruccion)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) +                         // identificador
                   sizeof(uint32_t) +                         // cant_parametros
                   espacio_de_array_parametros(instruccion) + // p1 + p2 + p3 + p4
                   sizeof(uint32_t) * 4;                      // p1_length, p2_length, p3_length, p4_length
    void *stream = malloc(buffer->size);
    int offset = 0;
    memcpy(stream + offset, &instruccion->identificador, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->cant_parametros, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->p1_length, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->p2_length, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->p3_length, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->p4_length, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    for (int i = 0; i < instruccion->cant_parametros; i++)
    {
        memcpy(stream + offset, instruccion->parametros[i], strlen(instruccion->parametros[i]) + 1);
        offset += strlen(instruccion->parametros[i]) + 1;
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
    }
    buffer->stream = stream;
    return buffer;
}

t_buffer *t_pcontexto_create_buffer(t_pcontexto *pcontexto)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    t_buffer *buffer_instrucciones = t_lista_instrucciones_create_buffer(pcontexto->instructions);

    buffer->size = sizeof(uint32_t) * 2 + buffer_instrucciones->size + sizeof(pcontexto->registers->AX) * 4 + sizeof(pcontexto->registers->EAX) * 4 + sizeof(pcontexto->registers->RAX) * 4;

    void *stream = malloc(sizeof(buffer->size));

    int offset = 0;

    memcpy(stream, &pcontexto->pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, buffer_instrucciones->stream, buffer_instrucciones->size);
    offset += buffer_instrucciones->size;
    memcpy(stream + offset, &pcontexto->program_counter, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &(pcontexto->registers->AX), sizeof(4));
    offset += sizeof(4);
    memcpy(stream + offset, &(pcontexto->registers->BX), sizeof(4));
    offset += sizeof(4);
    memcpy(stream + offset, &(pcontexto->registers->CX), sizeof(4));
    offset += sizeof(4);
    memcpy(stream + offset, &(pcontexto->registers->DX), sizeof(4));
    offset += sizeof(4);

    memcpy(stream + offset, &(pcontexto->registers->EAX), sizeof(8));
    offset += sizeof(8);
    memcpy(stream + offset, &(pcontexto->registers->EBX), sizeof(8));
    offset += sizeof(8);
    memcpy(stream + offset, &(pcontexto->registers->ECX), sizeof(8));
    offset += sizeof(8);
    memcpy(stream + offset, &(pcontexto->registers->EDX), sizeof(8));
    offset += sizeof(8);

    memcpy(stream + offset, &(pcontexto->registers->RAX), sizeof(16));
    offset += sizeof(16);
    memcpy(stream + offset, &(pcontexto->registers->RBX), sizeof(16));
    offset += sizeof(16);
    memcpy(stream + offset, &(pcontexto->registers->RCX), sizeof(16));
    offset += sizeof(16);
    memcpy(stream + offset, &(pcontexto->registers->RDX), sizeof(16));

    free(buffer_instrucciones);
    buffer->stream = stream;
    return buffer;
}

t_buffer *t_segment_create_buffer(t_segment *segment)
{   t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t) * 3;

    void *stream = malloc(buffer->size);

    int offset = 0;

    memcpy(stream + offset, &segment->id, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &segment->size, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &segment->base_adress, sizeof(uint32_t));

    buffer->stream = stream;
    return buffer;
}

t_buffer *t_segments_table_create_buffer(t_list *segments_table)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    uint32_t size_total = 0;
    for (int i = 0; i < list_size(segments_table); i++)
    {
        t_segment *segment = list_get(segments_table, i);
        t_buffer *buffer_segment = t_segment_create_buffer(segment);
        size_total += buffer_segment->size;
    }
    // creo el stream y copio los datos de cada buffer
    void *stream = malloc(size_total);
    buffer->size = size_total;
    uint32_t offset = 0;
    for (int i = 0; i < list_size(segments_table); i++)
    {
        t_buffer *buffer_segment = t_segment_create_buffer(list_get(segments_table, i));
        uint32_t size = buffer_segment->size;
        void *stream_segment = buffer_segment->stream;
        memcpy(stream + offset, stream_segment, size);
        offset += size;
    }
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

t_buffer *t_adress_create_buffer(t_adress adress)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t) * 1;

    void *stream = malloc(buffer->size);

    int offset = 0;

    memcpy(stream + offset, &adress, sizeof(uint32_t));

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

/* BUFFERS TIPOS DE DATOS -> RECV */

t_instruccion *t_instruccion_create_from_buffer(t_buffer *buffer, uint32_t *offset)
{
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    void *stream = buffer->stream;
    // printf("STREAM t_instruccion_create_from_buffer: %s\n", stream);
    uint32_t offset_aux = 0;
    stream += (*offset);

    memcpy(&(instruccion->identificador), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->cant_parametros), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->p1_length), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->p2_length), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->p3_length), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->p4_length), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    offset_aux += sizeof(uint32_t) * 6;
    instruccion->parametros = (char **)malloc(instruccion->cant_parametros * sizeof(char *));
    instruccion->parametros[0] = malloc(instruccion->p1_length);
    memcpy(instruccion->parametros[0], stream, instruccion->p1_length);
    offset_aux += instruccion->p1_length;
    stream += instruccion->p1_length;
    if (instruccion->p1_length == 1)
    {
        offset_aux -= 1;
        stream -= 1;
    }
    instruccion->parametros[1] = malloc(instruccion->p2_length);
    memcpy(instruccion->parametros[1], stream, instruccion->p2_length);
    offset_aux += instruccion->p2_length;
    stream += instruccion->p2_length;
    if (instruccion->p2_length == 1)
    {
        offset_aux -= 1;
        stream -= 1;
    }
    instruccion->parametros[2] = malloc(instruccion->p3_length);
    memcpy(instruccion->parametros[2], stream, instruccion->p3_length);
    offset_aux += instruccion->p3_length;
    stream += instruccion->p3_length;
    if (instruccion->p3_length == 1)
    {
        offset_aux -= 1;
        stream -= 1;
    }
    instruccion->parametros[3] = malloc(instruccion->p4_length);
    memcpy(instruccion->parametros[3], stream, instruccion->p4_length);
    offset_aux += instruccion->p4_length;
    stream += instruccion->p4_length;
    if (instruccion->p4_length == 1)
    {
        offset_aux -= 1;
        stream -= 1;
    }
    *offset += offset_aux;
    return instruccion;
}

t_list *t_lista_instrucciones_create_from_buffer(t_buffer *buffer)
{
    t_list *lista_instrucciones = list_create();
    uint32_t offset = 0;
    while (offset <= buffer->size)
    {
        t_instruccion *instruccion = t_instruccion_create_from_buffer(buffer, &offset);
        list_add(lista_instrucciones, instruccion);
    }
    return lista_instrucciones;
}

t_pcontexto *t_pcontexto_create_from_buffer(t_buffer *buffer)
{
    t_pcontexto *pcontexto = malloc(sizeof(t_pcontexto));
    void *stream = buffer->stream;

    memcpy(&(pcontexto->pid), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    // usar intrucciones_create_from_buffer
    // sumarle al stream la cantidad de bytes que ocupen las instrucciones

    memcpy(&(pcontexto->program_counter), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    memcpy(&(pcontexto->registers->AX), stream, sizeof(4));
    stream += sizeof(4);
    memcpy(&(pcontexto->registers->BX), stream, sizeof(4));
    stream += sizeof(4);
    memcpy(&(pcontexto->registers->CX), stream, sizeof(4));
    stream += sizeof(4);
    memcpy(&(pcontexto->registers->DX), stream, sizeof(4));
    stream += sizeof(4);

    memcpy(&(pcontexto->registers->EAX), stream, sizeof(8));
    stream += sizeof(8);
    memcpy(&(pcontexto->registers->EBX), stream, sizeof(8));
    stream += sizeof(8);
    memcpy(&(pcontexto->registers->ECX), stream, sizeof(8));
    stream += sizeof(8);
    memcpy(&(pcontexto->registers->EDX), stream, sizeof(8));
    stream += sizeof(8);

    memcpy(&(pcontexto->registers->RAX), stream, sizeof(16));
    stream += sizeof(16);
    memcpy(&(pcontexto->registers->RBX), stream, sizeof(16));
    stream += sizeof(16);
    memcpy(&(pcontexto->registers->RCX), stream, sizeof(16));
    stream += sizeof(16);
    memcpy(&(pcontexto->registers->RDX), stream, sizeof(16));

    free(buffer);
    return pcontexto;
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
    memcpy(&(segment->base_adress), stream, sizeof(uint32_t));

    *offset += sizeof(uint32_t) * 3;
    return segment;
}

t_list *t_segments_table_create_from_buffer(t_buffer *buffer)
{
    t_list *segments_table = list_create();
    uint32_t offset = 0;
    while (offset <= buffer->size)
    {
        t_segment *segment = t_segment_create_from_buffer(buffer, &offset);
        list_add(segments_table, segment);
    }
    return segments_table;
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

t_adress t_adress_create_from_buffer(t_buffer *buffer)
{
    t_adress adress;
    void *stream = buffer->stream;

    memcpy(&adress, stream, sizeof(uint32_t));

    return adress;
}

t_open_files *t_open_files_create_from_buffer(t_buffer *buffer)
{
    t_open_files *open_files = malloc(sizeof(t_open_files));
    // POR IMPLEMENTAR
    return open_files;
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
