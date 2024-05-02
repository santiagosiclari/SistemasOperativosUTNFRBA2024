#include <../include/consola.h>

void iniciar_proceso(t_buffer* buffer) {
	char* path = extraer_string_del_buffer(buffer);
	log_trace(kernel_logger, "Buffer(%d): [PATH:%s]", buffer->size, path);
	destruir_buffer(buffer);

	// Mandar a memoria
	t_buffer* enviar_a_memoria = crear_buffer();
	cargar_string_al_buffer(enviar_a_memoria, path);
	t_paquete* paquete = crear_paquete(INICIAR_PROCESO, enviar_a_memoria);
	enviar_paquete(paquete, fd_memoria);
	eliminar_paquete(paquete);
}

void atender_instruccion (char* leido) {
	char** comando_consola = string_split(leido, " ");
	t_buffer* buffer = crear_buffer();

	if(strcmp(comando_consola[0], "INICIAR_PROCESO") == 0) {
		cargar_string_al_buffer(buffer, comando_consola[1]); // Path
		iniciar_proceso(buffer);
	} else {
		log_error(kernel_logger, "ERROR");
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