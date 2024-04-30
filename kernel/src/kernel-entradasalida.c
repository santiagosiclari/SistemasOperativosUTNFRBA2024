#include "../include/kernel-entradasalida.h"

void conexion_kernel_entradasalida() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_entradasalida);
		switch (cod_op) {
		case MENSAJE:
			log_info(kernel_logger, "Mensaje recibido con el numero");
			break;
		case PAQUETE:
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
