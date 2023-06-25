#ifndef CLIENT_HANDLE_H_
#define CLIENT_HANDLE_H_

#include "client.h"
#include "utils.h"
#include "parser.h"
#include "types_controllers.h"

int start_console_client(char* ip_kernel, char* puerto_kernel, t_log* logger_console);
void send_instruccions_and_wait_to_end(t_config_console *config, t_list *lista_instrucciones, t_log *logger_console);
void wait_pid(int kernel_socket, t_log *logger_console);
void wait_end(int kernel_socket, t_log *logger_console);

#endif /* CLIENT_HANDLE_H_ */