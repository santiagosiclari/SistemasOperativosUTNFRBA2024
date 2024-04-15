#include <../include/consola.h>

void iniciar_consola(t_log* logger)
{
    char* leido;

	leido = readline("> ");
	log_info(logger,">> %s",leido);

	while (strcmp(leido,"") != 0)
	{
        free(leido);
		leido = readline("> ");
		log_info(logger,">> %s",leido);
	}
	
	free(leido);
}

