#include "server_handle.h"

void start_cpu_server(char *listen_port, t_log *logger)
{
   int server_socket = server_start(listen_port, logger);
   int client_socket;
   bool accept_connection = true;
   while (accept_connection)
   {
      client_socket = client_wait(server_socket, logger);
      if (client_socket != -1)
      {
         log_info(logger, "Se conecto un cliente");
         if (hs_server_to_module_valid(client_socket, HSCPU, logger))
            process_client(client_socket, logger);
         log_info(logger, "Cerrando conexion con cliente");
         socket_destroy(client_socket);
         socket_destroy(server_socket);
         accept_connection = false;
      }
   }
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
         registers_duplicate(contexto->registers, cpu_registers);
         t_pcontexto_desalojo *contexto_desalojo = execute_process(contexto);
         registers_duplicate(cpu_registers, contexto_desalojo->registers);
         bool res = send_pcontexto_desalojo(client_socket, contexto_desalojo, logger);
         pcontexto_desalojo_destroy(contexto_desalojo);
         pcontexto_destroy(contexto);
         if (!res)
         {
            log_error(logger, "El contexto no se pudo enviar al kernel");
            package_destroy(package);
            return;
         }
         log_info(logger, "Se envio el contexto al kernel");
         break;
      case END:
         log_info(logger, "Conexion Finalizada");
         exit = true;
         break;
      default:
         log_info(logger, "Operacion desconocida");
         exit = true;
         break;
      }
      package_destroy(package);
   }
}

void print_registers(t_registers *registers)
{
   for (int i = 0; i < 4; i++)
   {
      printf("%c", *(char *)(cpu_registers->AX + i));
   }
   printf("\n");
   for (int i = 0; i < 4; i++)
   {
      printf("%c", *(char *)(cpu_registers->BX + i));
   }
   printf("\n");
   for (int i = 0; i < 4; i++)
   {
      printf("%c", *(char *)(cpu_registers->CX + i));
   }
   printf("\n");
   for (int i = 0; i < 4; i++)
   {
      printf("%c", *(char *)(cpu_registers->DX + i));
   }
   printf("\n");
   for (int i = 0; i < 8; i++)
   {
      printf("%c", *(char *)(cpu_registers->EAX + i));
   }
   printf("\n");
   for (int i = 0; i < 8; i++)
   {
      printf("%c", *(char *)(cpu_registers->EBX + i));
   }
   printf("\n");
   for (int i = 0; i < 8; i++)
   {
      printf("%c", *(char *)(cpu_registers->ECX + i));
   }
   printf("\n");
   for (int i = 0; i < 8; i++)
   {
      printf("%c", *(char *)(cpu_registers->EDX + i));
   }
   printf("\n");
   for (int i = 0; i < 16; i++)
   {
      printf("%c", *(char *)(cpu_registers->RAX + i));
   }
   printf("\n");
   for (int i = 0; i < 16; i++)
   {
      printf("%c", *(char *)(cpu_registers->RBX + i));
   }
   printf("\n");
   for (int i = 0; i < 16; i++)
   {
      printf("%c", *(char *)(cpu_registers->RCX + i));
   }
   printf("\n");
   for (int i = 0; i < 16; i++)
   {
      printf("%c", *(char *)(cpu_registers->RDX + i));
   }
   printf("\n");
}