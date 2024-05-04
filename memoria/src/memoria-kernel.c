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
			char* path;
			if(!recv_iniciar_proceso(fd_kernel, &path)) {
				log_error(memoria_logger, "Hubo en error al recibir INICIAR_PROCESO");
				break;
			}

			log_info(memoria_logger, "Iniciando proceso del path: %s", path);

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