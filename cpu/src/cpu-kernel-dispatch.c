#include "../include/cpu-kernel-dispatch.h"

void conexion_cpu_kernel_dispatch() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op) {
		case RECIBIR_PCB:
			// Recibo PCB
			t_pcb* pcb = malloc(sizeof(t_pcb));
			pcb->registros = malloc(sizeof(t_registros));
			
			if(!recv_pcb(fd_kernel_dispatch, pcb)) {
				log_error(cpu_logger, "Hubo un error al recibir el PCB del modulo Kernel (Dispatch)");
				exit(EXIT_FAILURE);
			}

			// Controlo si es nulo
			if (pcb == NULL) {
				log_error(cpu_logger, "El PCB recibido es NULL");
				exit(EXIT_FAILURE);
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