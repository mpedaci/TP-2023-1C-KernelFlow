#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <assert.h>

#include "communication.h"

void *get_buffer(int *, int);

int start_server(char *);
int wait_client(int);
bool server_pass_handshake(int socket_cliente, t_log *logger);
t_list *get_package(int);
char *get_message(int);
int get_operation(int);

void client_destroy(int);
void server_destroy(int);

#endif /* SERVER_H_ */
