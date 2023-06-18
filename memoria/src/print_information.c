#include "print_information.h"

void print_menu()
{
    char line[2048];
    int i;
    int input = -1;
    printf("\n\nMenu:\n");
    printf("1. Mostrar config\n");
    printf("6. Salir\n");
    if (fgets(line, sizeof(line), stdin)) {
        if (1 == sscanf(line, "%d", &i)) {
            input = i;
        }
    }
    printf("\n\n");
    switch (input)
    {
    case 1:
        print_config();
        break;
    case 6:
        end_program_flag = true;
        break;
    default:
        printf("Opcion invalida\n");
        break;
    }
    sleep(1);
}

void print_config()
{
    printf("Configuracion:\n");
    printf("PUERTO_ESCUCHA: %s\n", config->port);
    printf("TAM_MEMORIA: %ld\n", config->memory_size);
    printf("TAM_SEGMENTO_0: %ld\n", config->segment_zero_size);
    printf("CANT_SEGMENTOS: %d\n", config->segment_quantity);
    printf("RETARDO_MEMORIA: %d\n", config->memory_time_delay);
    printf("RETARDO_COMPACTACION: %d\n", config->compactation_time_delay);
    printf("ALGORITMO_ASIGNACION: %s\n", config->compactation_algorithm);
}