#include "../include/memoria-kernel.h"
#define MAX_LENGTH 256

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
                char* path = malloc(MAX_LENGTH);

                if (!recv_iniciar_proceso(fd_kernel, path, MAX_LENGTH)) {
                    log_error(memoria_logger, "Hubo un error al recibir INICIAR_PROCESO");
                    free(path);
                    break;
                }

				log_info(memoria_logger, "Path recibido: %s", path);

                int required_length = strlen(PATH_INSTRUCCIONES) + strlen(path) + 1;  // +1 para el null-terminador
                char* path_concatenado = malloc(required_length);

                // Copia `PATH_INSTRUCCIONES` al nuevo bloque de memoria
				strcpy(path_concatenado, PATH_INSTRUCCIONES);
				// Concatenar `path` a `path_concatenado`
				strcat(path_concatenado, path);

				log_info(memoria_logger, "Path concatenado: %s", path_concatenado);

                // Aquí puedes usar `path_concatenado` para cualquier operación necesaria
				log_info(memoria_logger, "Instrucciones leidas del %s", path_concatenado);
                instrucciones = leer_archivo(path_concatenado);  // Usar la cadena concatenada para tu lógica

                // Libera la memoria después de usarla
                free(path);  // Libera el `path`
                free(path_concatenado);  // Libera el `path_concatenado`
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