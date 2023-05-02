#include "server_handle.h"


void start_cpu_server(char *listen_port, t_log *logger)
{
    int server_socket = server_start(listen_port, logger);
    int client_socket;

    client_socket = client_wait(server_socket, logger);
    log_info(logger, "Se conecto un cliente");

    if (hs_server_to_module_valid(client_socket, HSCPU, logger))
        process_client(client_socket, logger);
    
    socket_destroy(client_socket);
    socket_destroy(server_socket);
}

void process_client(int client_socket, t_log *logger) {
    bool exit = false;
    while (exit == false)
    {
        t_package* package = get_package(client_socket, logger);
        switch (package->operation_code)
        {
        case PCONTEXTO:
            t_pcontexto* contexto = get_pcontexto(package);
            set_registers(contexto->registers);
            contexto = execute_process(contexto);
            set_registers_contexto(contexto->registers);
            send_pcontexto(client_socket, contexto, logger);
            free_pcontexto(contexto);
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

void free_pcontexto(t_pcontexto* contexto) {
    // free t_list* instructions
    free(contexto->instructions->head);
    free(contexto->instructions->elements_count);

    // free registers pcontexto
    free(contexto->registers->AX);
    free(contexto->registers->BX);
    free(contexto->registers->CX);
    free(contexto->registers->DX);
    free(contexto->registers->EAX);
    free(contexto->registers->EBX);
    free(contexto->registers->ECX);
    free(contexto->registers->EDX);
    free(contexto->registers->RAX);
    free(contexto->registers->RBX);
    free(contexto->registers->RCX);
    free(contexto->registers->RDX);

    // free todo lo demas
    free(contexto);
}

void set_registers_cpu(t_registers *registers) {
    cpu_registers->AX = memcpy(cpu_registers->AX, registers->AX, sizeof(4));
    cpu_registers->AX = memcpy(cpu_registers->BX, registers->BX, sizeof(4));
    cpu_registers->AX = memcpy(cpu_registers->CX, registers->CX, sizeof(4));
    cpu_registers->AX = memcpy(cpu_registers->DX, registers->DX, sizeof(4));

    cpu_registers->AX = memcpy(cpu_registers->EAX, registers->EAX, sizeof(8));
    cpu_registers->AX = memcpy(cpu_registers->EBX, registers->EBX, sizeof(8));
    cpu_registers->AX = memcpy(cpu_registers->ECX, registers->ECX, sizeof(8));
    cpu_registers->AX = memcpy(cpu_registers->EDX, registers->EDX, sizeof(8));

    cpu_registers->AX = memcpy(cpu_registers->RAX, registers->RAX, sizeof(16));
    cpu_registers->AX = memcpy(cpu_registers->RBX, registers->RBX, sizeof(16));
    cpu_registers->AX = memcpy(cpu_registers->RCX, registers->RCX, sizeof(16));
    cpu_registers->AX = memcpy(cpu_registers->RDX, registers->RDX, sizeof(16));
}

void set_registers_contexto(t_registers *registers) {
    registers->AX = memcpy(registers->AX, cpu_registers->AX, sizeof(4));
    registers->AX = memcpy(registers->BX, cpu_registers->BX, sizeof(4));
    registers->AX = memcpy(registers->CX, cpu_registers->CX, sizeof(4));
    registers->AX = memcpy(registers->DX, cpu_registers->DX, sizeof(4));

    registers->AX = memcpy(registers->EAX, cpu_registers->EAX, sizeof(8));
    registers->AX = memcpy(registers->EBX, cpu_registers->EBX, sizeof(8));
    registers->AX = memcpy(registers->ECX, cpu_registers->ECX, sizeof(8));
    registers->AX = memcpy(registers->EDX, cpu_registers->EDX, sizeof(8));

    registers->AX = memcpy(registers->RAX, cpu_registers->RAX, sizeof(16));
    registers->AX = memcpy(registers->RBX, cpu_registers->RBX, sizeof(16));
    registers->AX = memcpy(registers->RCX, cpu_registers->RCX, sizeof(16));
    registers->AX = memcpy(registers->RDX, cpu_registers->RDX, sizeof(16));
}