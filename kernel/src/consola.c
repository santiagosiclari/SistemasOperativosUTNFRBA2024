#include <../include/consola.h>

void atender_instruccion (char* leido) {
	char** comando_consola = string_split(leido, " ");
	
	if(strcmp(comando_consola[0], "HELP") == 0) {
		printf( "\nInstrucciones:\n"
				"EJECUTAR_SCRIPT [path]\n"
				"INICIAR_PROCESO [path]\n"
				"FINALIZAR_PROCESO [pid]\n"
				"DETENER_PLANIFICACION\n"
				"INICIAR_PLANIFICACION\n"
				"MULTIPROGRAMACION [valor]\n"
				"INICIAR_PLANIFICACION\n"
				"PROCESO_ESTADO\n\n");
	} else if(strcmp(comando_consola[0], "INICIAR_PROCESO") == 0) {
		if(!send_iniciar_proceso(fd_memoria, comando_consola[1])) {
			log_error(kernel_logger, "Hubo un error al enviar INICIAR_PROCESO");
		}
	} else {
		log_error(kernel_logger, "ERROR. No se encontro el comando. Escribi HELP si necesitas ayuda con los comandos y sus parametros");
	}
}

void iniciar_consola(t_log* logger) {
    char* leido;
	leido = readline("> ");

	while (strcmp(leido,"") != 0)
	{
		atender_instruccion(leido);
        free(leido);
		leido = readline("> ");
	}
	
	free(leido);
}