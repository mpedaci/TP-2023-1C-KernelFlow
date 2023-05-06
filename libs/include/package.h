#ifndef PACKAGE_H_
#define PACKAGE_H_

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>

#include <sys/socket.h> // send

#include <commons/log.h>
#include <commons/string.h>

#include "types.h"

/* BUFFER */

typedef struct
{
    uint32_t size; // Tamaño del payload
    void *stream;  // Payload
} t_buffer;

/* BUFFERS TIPOS DE DATOS -> SEND */

uint32_t espacio_de_array_parametros(t_instruccion *instruccion);

t_buffer *t_instruccion_create_buffer(t_instruccion *instruccion);

uint32_t espacio_de_array_parametros(t_instruccion *instruccion);

t_buffer *t_lista_instrucciones_create_buffer(t_list *lista_instrucciones);

t_buffer *t_pcontexto_create_buffer(t_pcontexto *pcontexto);

t_buffer *t_segment_table_create_buffer(t_segments_table *segment_table);

t_buffer *t_data_create_buffer(t_data *data);

t_buffer *t_adress_create_buffer(t_adress adress);

t_buffer *t_open_files_create_buffer(t_open_files *open_files);

t_buffer *null_buffer();

/* BUFFERS TIPOS DE DATOS -> RECV */

t_instruccion *t_instruccion_create_from_buffer(t_buffer *buffer, uint32_t *offset);

t_list *t_lista_instrucciones_create_from_buffer(t_buffer *buffer);

t_pcontexto *t_pcontexto_create_from_buffer(t_buffer *buffer);

t_segments_table *t_segment_table_create_from_buffer(t_buffer *buffer);

t_data *t_data_create_from_buffer(t_buffer *buffer);

t_adress t_adress_create_from_buffer(t_buffer *buffer);

t_open_files *t_open_files_create_from_buffer(t_buffer *buffer);

/* PAQUETE */

typedef struct
{
    uint8_t operation_code;
    t_buffer *buffer;
} t_package;

t_package *package_create(t_buffer *buffer, int operation_code);
uint32_t package_get_size(t_package *paquete);

bool package_send(int socket, t_package *paquete, t_log *logger);
t_package *package_recv(int socket, t_log *logger);

void package_destroy(t_package *paquete);

#endif /* PACKAGE_H_ */
