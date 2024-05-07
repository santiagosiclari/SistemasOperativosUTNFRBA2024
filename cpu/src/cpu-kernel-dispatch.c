#include "../include/cpu-kernel-dispatch.h"

t_pcb* pcb = NULL;

void conexion_cpu_kernel_dispatch() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op) {
		case RECIBIR_PCB:
			if(!recv_pcb(fd_kernel_dispatch, &pcb)) {
				log_error(cpu_logger, "Hubo un error al recibir el PCB del modulo Kernel (Dispatch)");
				break;
			}
			if (pcb == NULL) {
				log_error(cpu_logger, "El PCB recibido es NULL");
				break;
			}
			break;
		case -1:
			log_error(cpu_logger, "El Kernel (Dispatch) se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(cpu_logger, "Operacion desconocida.");
			break;
		}
	}
}