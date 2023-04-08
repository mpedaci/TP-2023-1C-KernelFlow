#ifndef CLIENT_H_
#define CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;



int create_connection(char* ip, char* puerto);
void send_message(char* mensaje, int socket_cliente);
t_paquete* create_package(void);
void add_to_package(t_paquete* paquete, void* valor, int tamanio);
void send_package(t_paquete* paquete, int socket_cliente);
void destroy_connection(int socket_cliente);
void destroy_package(t_paquete* paquete);

#endif /* CLIENT_H_ */
