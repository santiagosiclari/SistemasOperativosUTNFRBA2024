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
		t_pcb* pcb = crear_pcb();
		if(!send_pcb(fd_cpu_dispatch, pcb)) {
			log_error(kernel_logger, "Hubo un error al INICIAR_PROCESO: Envio de PCB");
		} else {
			log_info(kernel_logger, "Se crea el proceso %d en NEW", pcb->pid);
		}

		if(!send_iniciar_proceso(fd_memoria, comando_consola[1], strlen(comando_consola[1]) + 1)) {
			log_error(kernel_logger, "Hubo un error al INICIAR_PROCESO: Envio de PATH");
		} else {
			log_info(kernel_logger, "Path enviado: %s", comando_consola[1]);
		}
	} else {
		log_warning(kernel_logger, "ERROR. No se encontro el comando. Escribi HELP si necesitas ayuda con los comandos y sus parametros");
	}

	// Liberar cada elemento
	int i = 0;
    while (comando_consola[i] != NULL) {
        free(comando_consola[i]);
        i++;
    }

    // Liberar el array
    free(comando_consola);
}

void iniciar_consola(t_log* logger) {
    char* leido;
	leido = readline("> ");

	while (strcmp(leido,"") != 0) {
		atender_instruccion(leido);
        free(leido);
		leido = readline("> ");
	}
	
	free(leido);
}