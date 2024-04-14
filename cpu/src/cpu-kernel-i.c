#include "../include/cpu-kernel-i.h"

void conexion_cpu_kernel_interrupt() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel_interrupt);
		switch (cod_op) {
		case MENSAJE:
			uint8_t nota1;
			if (!recv_mensaje(fd_kernel_interrupt, &nota1)) {
				log_error(cpu_logger, "Fallo recibiendo MENSAJE");
				break;
			}

			log_info(cpu_logger, "Mensaje recibido con el numero %" PRIu8 "!", nota1);
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(cpu_logger, "El Kernel - Dispatch se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(cpu_logger, "Operacion desconocida.");
			break;
		}
	}
}