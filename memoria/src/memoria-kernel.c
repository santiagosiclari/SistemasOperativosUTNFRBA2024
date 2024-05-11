#include "../include/memoria-kernel.h"

void conexion_memoria_kernel() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case INICIAR_PROCESO:
			uint32_t MAX_LENGTH = 128;
			char* path = malloc(MAX_LENGTH);
			char* path_recivido = malloc(MAX_LENGTH);
			char* path_instrucciones = PATH_INSTRUCCIONES;
			char* path_concatenado = malloc(MAX_LENGTH);

			// Recibe el path desde el kernel
			if (!recv_iniciar_proceso(fd_kernel, path_recivido)) {
				log_error(memoria_logger, "Hubo un error al recibir INICIAR_PROCESO");
				free(path);
				return;
			}

			strcpy(path, path_recivido);

			// Loguear el path concatenado
			log_info(memoria_logger, "Path recivido: %s", path);

			// Concatenar el PATH_INSTRUCCIONES con el path recibido
			snprintf(path_concatenado, MAX_LENGTH, "%s%s", path_instrucciones, path);  // Concatena

			// Loguear el path concatenado
			log_info(memoria_logger, "Path concatenado: %s", path_concatenado);

			// Leer el archivo y obtener las instrucciones (con la función que mencionaste)
			instrucciones = leer_archivo(path_concatenado);

			// Liberar espacio
			free(path_concatenado);
			free(path);
            break;
		case -1:
			log_error(memoria_logger, "El Kernel se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(memoria_logger, "Operacion desconocida.");
			break;
		}
	}
}