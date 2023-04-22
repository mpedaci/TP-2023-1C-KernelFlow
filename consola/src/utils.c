#include <utils.h>

t_config_console* read_config(char* config_path, t_log* logger_console){
    
    t_config_console* console_config = malloc(sizeof(t_config_console));
    t_config* config = config_create(config_path);

    if(config == NULL){
        log_error(logger_console, "NO se pudo leer el archivo de configuracion");
        return NULL;
    }

    console_config->ip_kernel = string_duplicate(config_get_string_value(config, "IP_KERNEL"));
    console_config->puerto_kernel = string_duplicate(config_get_string_value(config, "PUERTO_KERNEL"));

    log_debug(logger_console, "Archivo de configuracion leido correctamente");
    
    config_destroy(config);
    return console_config;
}

void end_program(t_log* logger_console, t_log* logger_aux, t_config_console* config){

    
    log_debug(logger_aux, "Finalizando programa");
    log_destroy(logger_console);
    log_destroy(logger_aux);

    free(config->ip_kernel);
    free(config->puerto_kernel);
    free(config);
}



