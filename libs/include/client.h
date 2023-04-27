#ifndef CLIENT_H_
#define CLIENT_H_

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

#include <string.h>

#include <commons/log.h>

#include "communication.h"

int create_connection(char* ip, char* puerto, t_log* logger);

#endif /* CLIENT_H_ */
