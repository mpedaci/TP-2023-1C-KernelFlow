#ifndef SERVER_H_
#define SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

extern t_log* logger;

void* get_buffer(int*, int);

int start_server(char*);
int wait_client(int);
t_list* get_package(int);
void get_message(int);
int get_operation(int);

void client_destroy(int);
void server_destroy(int);

#endif /* SERVER_H_ */
