#include "cpu.h"


int main() {
   // para cierre seguro
   signal(SIGINT, sigintHandler);
   
   // inicializo loggers
   logger = log_create(logger_path, "CPU", 1, LOG_LEVEL_INFO);
   logger_aux = log_create(logger_aux_path, "CPU_AUX", 1, LOG_LEVEL_DEBUG);

   // leer configuracion
   config = read_config(config_path, logger);
   if(config == NULL) {
      close_program_cpu(config, cpu_registers, socket_client_memoria, logger, logger_aux);
      return EXIT_FAILURE;
   }

   // print_config(config);

   cpu_registers = init_registers();

   // iniciar cliente memoria
   socket_client_memoria = start_memory_client(config->ip_memoria, config->puerto_memoria, logger_aux);
   if(socket_client_memoria == -1) {
      close_program_cpu(config, cpu_registers, socket_client_memoria, logger, logger_aux);
   }

   // inicializar servidor para kernel
   log_info(logger_aux, "Iniciando servidor");
   start_cpu_server(config->puerto_escucha, logger_aux);

   // fin del programa
   close_program_cpu(config, cpu_registers, socket_client_memoria, logger, logger_aux);
   return EXIT_SUCCESS;
}

void sigintHandler(int signum) {
    printf("\nIniciando fin del modulo por signal: %d\n", signum);
    close_program_cpu(config, cpu_registers, socket_client_memoria, logger, logger_aux);
    exit(signum);
}

void print_config(t_config_cpu *config) {
    printf("Configuracion:\n");
    printf("IP_MEMORIA: %s\n", config->ip_memoria);
    printf("PUERTO_MEMORIA: %s\n", config->puerto_memoria);
    printf("PUERTO_ESCUCHA: %s\n", config->puerto_escucha);
    printf("RETARDO_INSTRUCCION: %s\n", config->retardo_instruccion);
    printf("TAM_MAX_SEGMENTO: %s\n", config->tam_max_segmento);
}

void print_contexto(t_pcontexto *contexto) {
   printf("Printing pcontexto\n");
   printf("%d\n", contexto->pid);
   list_iterate(contexto->instructions, (void*) iterator_print_instruction);
   printf("%d\n", contexto->program_counter);
   print_registers(contexto->registers);
}

void print_contexto_desalojo(t_pcontexto_desalojo *contexto) {
   printf("Printing pcontexto\n");
   printf("%d\n", contexto->pid);
   printf("%d\n", contexto->program_counter);
   print_registers(contexto->registers);
   iterator_print_instruction(contexto->motivo_desalojo);
}

void iterator_print_instruction(t_instruccion *instruccion) {
   printf("%d - ", instruccion->identificador);
   printf("%d - ", instruccion->cant_parametros);
   printf("%d - ", instruccion->p1_length);
   printf("%d - ", instruccion->p2_length);
   printf("%d - ", instruccion->p3_length);
   printf("%d\n", instruccion->p4_length);

   for(int i=0; i<instruccion->cant_parametros; i++) {
      printf("%s\n", instruccion->parametros[i]);
   }
}

void print_registers(t_registers *registers) {
   for(int i=0; i<4; i++) {
      printf("%c", *(char *)(cpu_registers->AX + i));
   }
   printf("\n");
   for(int i=0; i<4; i++) {
      printf("%c", *(char *)(cpu_registers->BX + i));
   }
   printf("\n");
   for(int i=0; i<4; i++) {
      printf("%c", *(char *)(cpu_registers->CX + i));
   }
   printf("\n");
   for(int i=0; i<4; i++) {
      printf("%c", *(char *)(cpu_registers->DX + i));
   }
   printf("\n");
   for(int i=0; i<8; i++) {
      printf("%c", *(char *)(cpu_registers->EAX + i));
   }
   printf("\n");
   for(int i=0; i<8; i++) {
      printf("%c", *(char *)(cpu_registers->EBX + i));
   }
   printf("\n");
   for(int i=0; i<8; i++) {
      printf("%c", *(char *)(cpu_registers->ECX + i));
   }
   printf("\n");
   for(int i=0; i<8; i++) {
      printf("%c", *(char *)(cpu_registers->EDX + i));
   }
   printf("\n");
   for(int i=0; i<16; i++) {
      printf("%c", *(char *)(cpu_registers->RAX + i));
   }
   printf("\n");
   for(int i=0; i<16; i++) {
      printf("%c", *(char *)(cpu_registers->RBX + i));
   }
   printf("\n");
   for(int i=0; i<16; i++) {
      printf("%c", *(char *)(cpu_registers->RCX + i));
   }
   printf("\n");
   for(int i=0; i<16; i++) {
      printf("%c", *(char *)(cpu_registers->RDX + i));
   }
   printf("\n");
}