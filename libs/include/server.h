#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#include <commons/log.h>
#include <commons/collections/list.h>

#include <string.h>
#include <assert.h>

#include "communication.h"

int server_start(char *, t_log *);
int client_wait(int, t_log *);

#endif /* SERVER_H_ */
