#include "server_handle.h"

void start_cpu_server(char *listen_port, t_log *logger)
{
    int server_socket = server_start(listen_port, logger);
    int client_socket;

    client_socket = client_wait(server_socket, logger);
    log_info(logger, "Se conecto un cliente");

    if (hs_server_to_module_valid(client_socket, HSCPU, logger))
        process_client(client_socket, logger);

    log_info(logger, "Cerrando conexion con cliente");
    socket_destroy(client_socket);
    socket_destroy(server_socket);
}

void process_client(int client_socket, t_log *logger)
{
    bool exit = false;
    while (exit == false)
    {
        t_package *package = get_package(client_socket, logger);
        switch (package->operation_code)
        {
        case PCONTEXTO:
            t_pcontexto *contexto = get_pcontexto(package);
            log_info(logger, "Se recibio un contexto");
            log_info(logger, "Pid: %d", contexto->pid);
            log_info(logger, "Instrucciones: %d", list_size(contexto->instructions));

            /* for (int i = 0; i < list_size(contexto->instructions); i++)
            {
                t_instruccion *instruccion = list_get(contexto->instructions, i);
                log_debug(logger, "Instruccion: %d", instruccion->identificador);
                for (int j = 0; j < instruccion->cant_parametros; j++)
                    log_debug(logger, "Parametro %d: %s", j, (char *)list_get(instruccion->parametros, j));
            } */

            copy_registers(cpu_registers, contexto->registers);
            t_pcontexto_desalojo *contexto_desalojo = execute_process(contexto);
            log_warning(logger, "HOLA TEST 2");
            copy_registers(contexto_desalojo->registers, cpu_registers);
            bool res = send_pcontexto_desalojo(client_socket, contexto_desalojo, logger);
            free_pcontexto(contexto);
            free_pcontexto_desalojo(contexto_desalojo);
            if (!res)
            {
                log_error(logger, "El contexto no se pudo enviar al kernel");
                package_destroy(package);
                return;
            }
            break;
        case END:
            printf("Conexion Finalizada");
            exit = true;
            break;
        default:
            printf("Operacion desconocida.");
            exit = true;
            break;
        }
        package_destroy(package);
    }
}

void free_pcontexto(t_pcontexto *contexto)
{
    // free t_list* instructions
    list_destroy_and_destroy_elements(contexto->instructions, (void *)instruction_destroyer);

    // free registers pcontexto
    registers_destroy(contexto->registers);

    // free todo lo demas
    free(contexto);
}

void free_pcontexto_desalojo(t_pcontexto_desalojo *contexto)
{
    // free t_list* instructions
    list_destroy_and_destroy_elements(contexto->instructions, (void *)instruction_destroyer);

    // free registers pcontexto
    registers_destroy(contexto->registers);

    // free motivo_desalojo
    instruction_destroyer(contexto->motivo_desalojo);

    // free todo lo demas
    free(contexto);
}
