#include "memoria.h"

int main(void)
{
	// INICIALIZAR LOGS
	logger_main = log_create(logger_main_path, "Memoria", 1, LOG_LEVEL_DEBUG);
	logger_aux = log_create(logger_aux_path, "Memoria Aux", 1, LOG_LEVEL_DEBUG);

	// INICIALIZAR MEMORIA
	config = NULL;
	config = read_config(config_path);
	if (config == NULL)
	{
		end_program(logger_main, logger_aux, config);
		return EXIT_FAILURE;
	}

	start_memory(config);

	// INICIALIZO SERVIDOR
	start_memory_server(config->port);

	end_program(logger_main, logger_aux, config);

	return EXIT_SUCCESS;
};