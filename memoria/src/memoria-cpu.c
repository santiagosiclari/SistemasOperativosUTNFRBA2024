#include "../include/memoria-cpu.h"

void conexion_memoria_cpu() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_cpu);
		switch (cod_op) {
		case MENSAJE:
			log_info(memoria_logger, "Mensaje recibido con el numero");
			break;
		case PAQUETE:
			break;
		case RECIBIR_PC:
			uint32_t pc;
			if(!recv_pc(fd_cpu, &pc)) {
				log_error(memoria_logger, "Hubo un error al recibir el PC del modulo de CPU");
			} else {
				log_info(memoria_logger, "PC (Program Counter) recibido: %d", pc);
			}
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