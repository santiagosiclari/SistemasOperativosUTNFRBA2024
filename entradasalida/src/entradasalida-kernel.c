#include "../include/entradasalida-kernel.h"

void conexion_entradasalida_kernel() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(entradasalida_logger, "El servidor de Kernel no se encuentra activo.");
			control = 0;
            break;
		default:
			log_warning(entradasalida_logger, "Operacion desconocida.");
			break;
		}
	}
}