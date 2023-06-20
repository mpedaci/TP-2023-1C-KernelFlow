#include "utils.h"

t_config_cpu* read_config(char* path, t_log* logger) {

    t_config_cpu* config_cpu = malloc(sizeof(t_config_cpu));
    t_config* config = config_create(path);

    if(config == NULL) {
        log_error(logger, "No se pudo leer el archivo de configuracion");
        return NULL;
    }

    config_cpu->ip_memoria = string_duplicate(config_get_string_value(config, "IP_MEMORIA"));
    config_cpu->puerto_memoria = string_duplicate(config_get_string_value(config, "PUERTO_MEMORIA"));
    config_cpu->puerto_escucha = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA"));
    config_cpu->tam_max_segmento = string_duplicate(config_get_string_value(config, "TAM_MAX_SEGMENTO"));
    config_cpu->retardo_instruccion = string_duplicate(config_get_string_value(config, "RETARDO_INSTRUCCION"));

    log_debug(logger, "Archivo de configuracion se leyo correctamente");

    config_destroy(config);
    return config_cpu;
}


void close_program_cpu(t_config_cpu* config, t_registers* registers, int socket_client_memoria, t_log* logger, t_log* logger_aux) {
    // destruyo loggers
    log_destroy(logger);
    log_destroy(logger_aux);

    // destruyo y libero config
    send_end(socket_client_memoria, logger_aux);
    free(config->ip_memoria);
    free(config->puerto_memoria);
    free(config->puerto_escucha);
    free(config->tam_max_segmento);
    free(config->retardo_instruccion);
    free(config);

    // free registers
    registers_destroy(registers);

    // destruyo socket memoria
    socket_destroy(socket_client_memoria);
}

t_registers* init_registers() {
    t_registers* registers = malloc(sizeof(t_registers));

    registers->AX = malloc(4);
    registers->BX = malloc(4);
    registers->CX = malloc(4);
    registers->DX = malloc(4);
    registers->EAX = malloc(8);
    registers->EBX = malloc(8);
    registers->ECX = malloc(8);
    registers->EDX = malloc(8);
    registers->RAX = malloc(16);
    registers->RBX = malloc(16);
    registers->RCX = malloc(16);
    registers->RDX = malloc(16);

    set_registers_zero(registers);

    return registers;
}

void set_registers_zero(t_registers *registers) {
    char *zero = "0000000000000000";

    memcpy(registers->AX, zero, 4);
    memcpy(registers->BX, zero, 4);
    memcpy(registers->CX, zero, 4);
    memcpy(registers->DX, zero, 4);
    memcpy(registers->EAX, zero, 8);
    memcpy(registers->EBX, zero, 8);
    memcpy(registers->ECX, zero, 8);
    memcpy(registers->EDX, zero, 8);
    memcpy(registers->RAX, zero, 16);
    memcpy(registers->RBX, zero, 16);
    memcpy(registers->RCX, zero, 16);
    memcpy(registers->RDX, zero, 16);
}

int get_sizeof_register(char *reg) {
    if (strcmp(reg, "AX") == 0 || strcmp(reg, "BX") == 0 || strcmp(reg, "CX") == 0 || strcmp(reg, "DX") == 0)
    {
        return 4;
    }
    else if (strcmp(reg, "EAX") == 0 || strcmp(reg, "EBX") == 0 || strcmp(reg, "ECX") == 0 || strcmp(reg, "EDX") == 0)
    {
        return 8;
    }
    else if (strcmp(reg, "RAX") == 0 || strcmp(reg, "RBX") == 0 || strcmp(reg, "RCX") == 0 || strcmp(reg, "RDX") == 0)
    {
        return 16;
    }
    else
    {
        return -1;
    }
}