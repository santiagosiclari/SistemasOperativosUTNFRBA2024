#include "../include/kernel-cpu-dispatch.h"

char* nombre_interfaz;

pthread_mutex_t mutexIO = PTHREAD_MUTEX_INITIALIZER;

void conexion_kernel_cpu_dispatch() {
	uint8_t MAX_LENGTH = 128;
	nombre_interfaz = malloc(MAX_LENGTH);
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_cpu_dispatch);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case RECIBIR_PID_A_BORRAR:
			// Recibo proceso a eliminar
			uint8_t pid_a_borrar;
			if(!recv_pid_a_borrar(fd_cpu_dispatch, &pid_a_borrar)) {
				log_error(kernel_logger, "Hubo un error al recibir el PID.");
			} else {
				log_info(kernel_logger, "Proceso a finalizar: %d", pid_a_borrar);
			}

    		pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				queue_pop(colaExec);
				if (queue_is_empty(colaReady) && queue_is_empty(colaNew) && queue_is_empty(colaExec) && queue_is_empty(colaBlocked)) {
					control_planificacion = 0; // No hay más procesos pendientes, termina la planificación
				}
			}
			pthread_mutex_unlock(&colaExecMutex);

			log_info(kernel_logger, "Proceso eliminado");

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
			if(queue_size(colaExec) != 0) {
				queue_pop(colaExec);
			}
			pthread_mutex_unlock(&colaExecMutex);

			pthread_mutex_lock(&colaReadyMutex);
			pcb_interrumpido->estado = 'R';
			queue_push(colaReady, pcb_interrumpido);
			pthread_mutex_unlock(&colaReadyMutex);
			break;
		case IO_GEN_SLEEP:
			pthread_mutex_lock(&mutexIO);
			t_pcb* pcb_io_gen_sleep = malloc(sizeof(t_pcb));
			pcb_io_gen_sleep->registros = malloc(sizeof(t_registros));
			uint32_t unidades_de_trabajo;
			char* nombre_recibido = malloc(MAX_LENGTH);

			if(!recv_io_gen_sleep(fd_cpu_dispatch, pcb_io_gen_sleep, &unidades_de_trabajo, nombre_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_GEN_SLEEP");
			}
			// No es necesario de utilizar --> Esta asi para que la IO sepa que proceso interrumpir
			free(pcb_io_gen_sleep->registros);
			free(pcb_io_gen_sleep);
			strcpy(nombre_interfaz, nombre_recibido);
			
			// Busca el socket de la interfaz
			int fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_interfaz);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "Iniciando interrupcion del proceso %d", pcb_recibido->pid);
				if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0) {
					log_info(kernel_logger, "Se recibio una IO antes del Quantum");
					pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
				}
				pthread_mutex_lock(&colaBlockedMutex);
				pcb_recibido->estado = 'B';
				queue_push(colaBlocked, pcb_recibido);
				pthread_mutex_unlock(&colaBlockedMutex);
				// Envia PCB y lo necesario para la IO
				send_io_gen_sleep(fd_interfaz, pcb_recibido, unidades_de_trabajo, nombre_interfaz, strlen(nombre_interfaz) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);

			pthread_mutex_unlock(&mutexIO);

            free(nombre_recibido);
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
	free(nombre_interfaz);
}