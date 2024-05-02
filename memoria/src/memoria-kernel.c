#include "../include/memoria-kernel.h"

void conexion_memoria_kernel() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op) {
		case MENSAJE:
			log_info(memoria_logger, "Mensaje recibido con el numero");
			break;
		case PAQUETE:
			break;
		case INICIAR_PROCESO:
			log_info(memoria_logger,"Ta todo bien");
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