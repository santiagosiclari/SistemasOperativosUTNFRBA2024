#include "../include/kernel-entradasalida.h"

void conexion_kernel_entradasalida() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_entradasalida);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case FIN_IO:
			uint32_t MAX_LENGTH = 128;
			char* nombre = malloc(MAX_LENGTH);
			char* nombre_recivido = malloc(MAX_LENGTH);

			if(!recv_fin_io(fd_entradasalida, nombre_recivido)) {
				log_error(kernel_logger, "Hubo un error al recibir el fin de una IO");
			}
			strcpy(nombre, nombre_recivido);

			log_info(kernel_logger, "Fin de IO de la interfaz %s del proceso", nombre); //, proceso_block->pid);
			// queue_pop(colaBlocked);
			// queue_push(colaReady, proceso_block);

			break;
		case -1:
			log_error(kernel_logger, "El IO se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(kernel_logger, "Operacion desconocida.");
			break;
		}
	}
}