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


void close_program_cpu(t_config_cpu* config, t_log* logger, t_log* logger_aux) {
    // destruyo loggers
    log_destroy(logger);
    log_destroy(logger_aux);

    // destruyo y libero config
    free(config->ip_memoria);
    free(config->puerto_memoria);
    free(config->puerto_escucha);
    free(config->tam_max_segmento);
    free(config->retardo_instruccion);
    free(config);
}