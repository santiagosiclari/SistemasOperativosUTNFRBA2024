#include "../include/kernel-cpu-dispatch.h"

void conexion_kernel_cpu_dispatch() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_cpu_dispatch);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case RECIBIR_PID_A_BORRAR:
			// Recibo proceso a ejecutar
			uint8_t pid_a_borrar;
			if(!recv_pid_a_borrar(fd_cpu_dispatch, &pid_a_borrar)) {
				log_error(kernel_logger, "Hubo un error al recibir el PID.");
			} else {
				log_info(kernel_logger, "Proceso a finalizar: %d", pid_a_borrar);
			}

			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_borrado = queue_pop(colaExec);
				free(pcb_borrado);
			}
    		pthread_mutex_unlock(&colaExecMutex); // Desbloquea la colaExec
			break;
		case IO_GEN_SLEEP:
			uint32_t MAX_LENGTH = 128;
			uint32_t unidades_de_trabajo;
			char* nombre_interfaz = malloc(MAX_LENGTH);
			char* nombre_recivido = malloc(MAX_LENGTH);
			if(!recv_io_gen_sleep(fd_cpu_dispatch, &unidades_de_trabajo, nombre_recivido)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_GEN_SLEEP");
			}

			// send a la io
			strcpy(nombre_interfaz, nombre_recivido);

			free(nombre_interfaz);
            free(nombre_recivido);
			break;
		case -1:
			log_error(kernel_logger, "El servidor de CPU (Dispatch) no se encuentra activo.");
			control = 0;
            break;
		default:
			log_warning(kernel_logger, "Operacion desconocida.");
			break;
		}
	}
}