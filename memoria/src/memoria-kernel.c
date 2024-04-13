#include "../include/memoria-kernel.h"

void conexion_memoria_kernel() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_cpu_dispatch);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(memoria_logger, "El CPU se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(memoria_logger, "Operacion desconocida.");
			break;
		}
	}
}