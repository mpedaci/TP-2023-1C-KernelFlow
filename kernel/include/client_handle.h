#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H

#include <commons/log.h>

#include "kernel_structs.h"

#include "client.h"
#include "communication.h"

t_modules_client *start_modules_client(t_config_kernel *config, t_log *logger);
int start_client(char *ip, char *port, hs_code hsmodule, t_log *logger);

#endif /* CLIENT_HANDLE_H */
