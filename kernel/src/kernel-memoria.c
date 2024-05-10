#include "../include/kernel-memoria.h"

void conexion_kernel_memoria() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(kernel_logger, "El servidor de Memoria no se encuentra activo.");
			control = 0;
            break;
		default:
			log_warning(kernel_logger, "Operacion desconocida.");
			break;
		}
	}
}