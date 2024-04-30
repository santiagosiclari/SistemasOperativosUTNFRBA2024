#include "../include/cpu-kernel-dispatch.h"

void conexion_cpu_kernel_dispatch() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel_interrupt);
		switch (cod_op) {
		case MENSAJE:
			log_info(cpu_logger, "Mensaje recibido con el numero");
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(cpu_logger, "El IO se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(cpu_logger, "Operacion desconocida.");
			break;
		}
	}
}
