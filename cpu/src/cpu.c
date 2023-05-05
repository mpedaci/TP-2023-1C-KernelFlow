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
      close_program_cpu(config, cpu_registers, logger, logger_aux);
      return EXIT_FAILURE;
   }

   // print_config(config);

   cpu_registers = init_registers();

   // inicializar servidor para kernel
   log_info(logger_aux, "Iniciando servidor");
   start_cpu_server(config->puerto_escucha, logger_aux);

   // fin del programa
   close_program_cpu(config, cpu_registers, logger, logger_aux);
   return EXIT_SUCCESS;
}

void sigintHandler(int signum) {
    printf("\nIniciando fin del modulo por signal: %d\n", signum);
    close_program_cpu(config, cpu_registers, logger, logger_aux);
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
   // list_iterate(contexto->instructions, (void*) iterator_print_instruction);
   printf("%d\n", contexto->program_counter);
   print_registers(contexto->registers);
}

void iterator_print_instruction(t_instruccion *instruccion) {
   printf("%d\n", instruccion->identificador);
   printf("%d\n", instruccion->cant_parametros);
   printf("%d\n", instruccion->p1_length);
   printf("%d\n", instruccion->p2_length);
   printf("%d\n", instruccion->p3_length);
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

t_instruccion *new_instruction(t_identificador identificador, t_list *parametros)
{
    t_instruccion *tmp = malloc(sizeof(t_instruccion));

    tmp->identificador = identificador;
    if (parametros == NULL)
    {
        tmp->cant_parametros = 0;
        tmp->parametros = NULL;
    }
    else
    {
        tmp->cant_parametros = list_size(parametros);
        tmp->parametros = malloc(sizeof(char *) * tmp->cant_parametros);
        add_param_to_instruction(parametros, tmp);
    }

    return tmp;
}

void add_param_to_instruction(t_list *parametros, t_instruccion *instruccion)
{
    if (parametros != NULL)
        for(int i=0; i < instruccion->cant_parametros; i++)
        {
            instruccion->parametros[i] = list_get(parametros, i);
        }
    instruccion->p1_length = 1;
    instruccion->p2_length = 1;
    instruccion->p3_length = 1;
    instruccion->p4_length = 1;
    instruccion->p1_length = strlen(instruccion->parametros[0]) + 1;
    if (instruccion->cant_parametros >= 2)
        instruccion->p2_length = strlen(instruccion->parametros[1]) + 1;
    if (instruccion->cant_parametros >= 3)
        instruccion->p3_length = strlen(instruccion->parametros[2]) + 1;
    if (instruccion->cant_parametros >= 4)
        instruccion->p4_length = strlen(instruccion->parametros[3]) + 1;
}