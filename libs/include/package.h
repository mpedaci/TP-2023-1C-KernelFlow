#ifndef PACKAGE_H_
#define PACKAGE_H_

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>

#include <sys/socket.h> // send

#include <commons/log.h>

#include "types.h"


/* BUFFER */

typedef struct {
    uint32_t size; // Tamaño del payload
    void* stream; // Payload
} t_buffer;

/* BUFFERS TIPOS DE DATOS -> SEND */

uint32_t espacio_de_array_parametros(t_instruccion* instruccion);

t_buffer* t_lista_instrucciones_create_buffer(t_list* lista_instrucciones);

t_buffer* t_persona_create_buffer(t_persona persona);

t_buffer* null_buffer();

/* BUFFERS TIPOS DE DATOS -> RECV */

t_list* t_lista_instrucciones_create_from_buffer(t_buffer* buffer);

t_persona* t_persona_create_from_buffer(t_buffer* buffer);

/* PAQUETE */

typedef struct {
    uint8_t operation_code;
    t_buffer* buffer;
} t_package;


t_package* package_create(t_buffer *buffer, int operation_code);
uint32_t package_get_size(t_package* paquete);

bool package_send(int socket, t_package* paquete, t_log* logger);
t_package* package_recv(int socket, t_log* logger);

void package_destroy(t_package* paquete);


#endif /* PACKAGE_H_ */
