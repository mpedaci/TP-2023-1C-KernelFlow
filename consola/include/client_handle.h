#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H

#include "client.h"
#include "utils.h"
#include "parser.h"

int start_console_client(char* ip_kernel, char* puerto_kernel, t_log* logger_console);
void send_instruccions_and_wait_answer(t_config_console *config, t_list *lista_instrucciones, t_log *logger_console);

#endif