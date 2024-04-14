#include "../include/memoria-cpu.h"

void conexion_memoria_cpu() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_cpu);
		switch (cod_op) {
		case MENSAJE:
			uint8_t nota1;
			if (!recv_mensaje(fd_cpu, &nota1)) {
				log_error(memoria_logger, "Fallo recibiendo MENSAJE");
				break;
			}

			log_info(memoria_logger, "Mensaje recibido con el numero %" PRIu8 "!", nota1);
			break;
		case PAQUETE:
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