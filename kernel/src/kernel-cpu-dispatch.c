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
    			pthread_mutex_lock(&colaExecMutex);
				t_pcb* pcb_borrado = queue_pop(colaExec);
    			pthread_mutex_unlock(&colaExecMutex);
				free(pcb_borrado);
			}
			break;
		case RECIBIR_PCB:
			// PCB interrumpido por fin de quantum
			t_pcb* pcb_interrumpido = malloc(sizeof(t_pcb));
			pcb_interrumpido->registros = malloc(sizeof(t_pcb));

			if(!recv_pcb(fd_cpu_dispatch, pcb_interrumpido)) {
				log_error(kernel_logger, "Hubo un error al recibir el PCB interrumpido");
				break;
			}

			pthread_mutex_lock(&colaExecMutex);
			queue_pop(colaExec);
			pthread_mutex_unlock(&colaExecMutex);

			pthread_mutex_lock(&colaReadyMutex);
			pcb_interrumpido->estado = 'R';
			queue_push(colaReady, pcb_interrumpido);
			pthread_mutex_unlock(&colaReadyMutex);
			break;
		case IO_GEN_SLEEP:
			t_pcb* pcb_io_gen_sleep = malloc(sizeof(t_pcb));
			pcb_io_gen_sleep->registros = malloc(sizeof(t_registros));
			uint32_t MAX_LENGTH = 128;
			uint32_t unidades_de_trabajo;
			char* nombre_interfaz = malloc(MAX_LENGTH);
			char* nombre_recivido = malloc(MAX_LENGTH);
			if(!recv_io_gen_sleep(fd_cpu_dispatch, pcb_io_gen_sleep, &unidades_de_trabajo, nombre_recivido)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_GEN_SLEEP");
			}

			strcpy(nombre_interfaz, nombre_recivido);
			log_info(kernel_logger, "Iniciando interrupcion del proceso %d", pcb_io_gen_sleep->pid);

            pthread_mutex_lock(&colaExecMutex);
			queue_pop(colaExec);
			pthread_mutex_unlock(&colaExecMutex);

			pthread_mutex_lock(&colaBlockedMutex);
			queue_push(colaBlocked, pcb_io_gen_sleep);
			pthread_mutex_unlock(&colaBlockedMutex);

			send_io_gen_sleep(fd_entradasalida, pcb_io_gen_sleep, unidades_de_trabajo, nombre_interfaz, strlen(nombre_interfaz) + 1);

			free(nombre_interfaz);
            free(nombre_recivido);
			free(pcb_io_gen_sleep->registros);
            free(pcb_io_gen_sleep);
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