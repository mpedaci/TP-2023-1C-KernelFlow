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
            log_info(logger, "Se recibio un contexto | Pid: %d", contexto->pid);
            copy_registers(cpu_registers, contexto->registers);
            t_pcontexto_desalojo *contexto_desalojo = execute_process(contexto);
            copy_registers(contexto_desalojo->registers, cpu_registers);
            bool res = send_pcontexto_desalojo(client_socket, contexto_desalojo, logger);
            free_pcontexto_desalojo(contexto_desalojo);
            free_pcontexto(contexto);
            if (!res)
            {
                log_error(logger, "El contexto no se pudo enviar al kernel");
                package_destroy(package);
                return;
            }
            
            log_info(logger, "Se envio el contexto al kernel");
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
