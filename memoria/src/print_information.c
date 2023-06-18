#include "print_information.h"

void print_menu()
{
    char line[2048];
    int i;
    int input = -1;
    printf("\n\nMenu:\n");
    printf("1. Mostrar config\n");
    printf("2. Mostrar tabla de segmentos\n");
    printf("3. Mostrar tabla de huecos libres\n");
    printf("4. Mostrar contenido de memoria\n");
    printf("5. Mostrar estados internos\n");
    printf("6. Salir\n");
    if (fgets(line, sizeof(line), stdin))
    {
        if (1 == sscanf(line, "%d", &i))
        {
            input = i;
        }
    }
    printf("\n\n");
    switch (input)
    {
    case 1:
        print_config();
        break;
    case 2:
        input = -1;
        printf("Ingrese el PID de la tabla de segmentos: ");
        if (fgets(line, sizeof(line), stdin))
            if (1 == sscanf(line, "%d", &i))
                input = i;
        t_segments_table *segments_table = get_segments_table_by_pid(input);
        if (segments_table == NULL)
            printf("No existe la tabla de segmentos para el PID: %d\n", input);
        else
            print_segments_tables(segments_table);
        break;
    case 3:
        print_free_space_table();
        break;
    case 4:
        print_all_memory();
        break;
    case 5:
        print_status_memory();
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
    printf("  PUERTO_ESCUCHA: %s\n", config->port);
    printf("  TAM_MEMORIA: %ld\n", config->memory_size);
    printf("  TAM_SEGMENTO_0: %ld\n", config->segment_zero_size);
    printf("  CANT_SEGMENTOS: %d\n", config->segment_quantity);
    printf("  RETARDO_MEMORIA: %d\n", config->memory_time_delay);
    printf("  RETARDO_COMPACTACION: %d\n", config->compactation_time_delay);
    printf("  ALGORITMO_ASIGNACION: %s\n", config->compactation_algorithm);
}

void print_segments_tables(t_segments_table *segments_table)
{
    printf("Tabla de segmentos del PID: %d\n", segments_table->pid);
    printf("  ID\tBASE\tSIZE\n");
    for (int i = 0; i < segments_table->segment_list->elements_count; i++)
    {
        t_segment *segment = list_get(segments_table->segment_list, i);
        printf("  %d\t%d\t%d\n", segment->id, segment->base_address, segment->size);
    }
}

void print_free_space_table()
{
    printf("Tabla de huecos libres\n");
    printf("  BASE\tSIZE\n");
    for (int i = 0; i < free_space_table->size; i++)
    {
        if (!bitarray_get(free_space_table, i))
        {
            int size = get_free_space(i);
            printf("  %d\t%d\n", i, size);
            i += size - 1;
        }
    }
}

void print_all_memory()
{
    printf("Contenido de memoria\n");
    // Mostrar sesgmento 0
    printf("Segmento 0\n");
    printf("  ID\tBASE\tSIZE\n");
    printf("  %d\t%d\t%ld\n", 0, 0, config->segment_zero_size);
    for (int i = 0; i < list_size(all_segments_tables); i++)
    {
        t_segments_table *segments_table = list_get(all_segments_tables, i);
        printf("PID: %d\n", segments_table->pid);
        print_segments_tables(segments_table);
    }
}

void print_status_memory()
{
    printf("Estados internos\n");
    printf("end_program_flag: %d\n", end_program_flag);
    printf("accept_connections: %d\n", accept_connections);
}