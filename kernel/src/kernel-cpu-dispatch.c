#include "../include/kernel-cpu-dispatch.h"

char* nombre_interfaz;
int fd_interfaz;

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
				send_fin_proceso(fd_memoria, pid_a_borrar);
			}

    		pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_a_borrar = queue_pop(colaExec);
				// Revisar si otro proceso se puede desbloquear
				liberar_recursos(pid_a_borrar);

				if (queue_is_empty(colaReady) && queue_is_empty(colaNew) && queue_is_empty(colaExec) && queue_is_empty(colaBlocked) && queue_is_empty(colaAux)) {
					control_planificacion = 0; // No hay más procesos pendientes, termina la planificación
				}
				free(pcb_a_borrar->registros);
				free(pcb_a_borrar);
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
		case WAIT:
			t_pcb* pcb_wait = malloc(sizeof(t_pcb));
			pcb_wait->registros = malloc(sizeof(t_registros));
			char* recurso_wait = malloc(MAX_LENGTH);
			char* recurso_wait_recibido = malloc(MAX_LENGTH);

			if(!recv_wait_signal(fd_cpu_dispatch, pcb_wait, recurso_wait_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir el WAIT");
			}
			strcpy(recurso_wait, recurso_wait_recibido);
			string_trim_right(&recurso_wait);

			for(int i = 0; i < list_size(recursos); i++) {
				t_recurso* r = list_get(recursos, i);
				if(strcmp(recurso_wait, r->nombre) == 0) {
					log_info(kernel_logger, "Recurso %s encontrado - Instancias %d", r->nombre, r->instancias);
					if(r->instancias <= 0) {
						log_info(kernel_logger, "NO hay recursos para %s", r->nombre);
						// El proceso se bloquea en la cola correspondiente dependiendo el recurso tomado
           				pthread_mutex_lock(&colaExecMutex);
						if(!queue_is_empty(colaExec)) {
							// No hay recursos necesarias para que siga ejecutando
							send_recursos_ok(fd_cpu_dispatch, -1);
							
							t_pcb* pcb_recv = queue_pop(colaExec);
							if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0) {
								log_info(kernel_logger, "Se bloqueo por falta de Recursos");
								pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
							} else if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
								log_info(kernel_logger, "Se bloqueo por falta de Recursos");
								pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
								// Resto el tiempo tomado de time.h con el pcb_recibido->quantum
								uint32_t tiempo_restante = temporal_gettime(tiempo_vrr);
								pcb_recv->quantum -= tiempo_restante;
								log_info(kernel_logger, "Tiempo restante de quantum: %d", tiempo_restante);

								temporal_destroy(tiempo_vrr);

								if (pcb_recv->quantum <= 0) {
									pcb_recv->quantum = QUANTUM;
								}
							}
							
							pcb_recv->estado = 'B';
							queue_push(r->blocked, pcb_recv);
						}
            			pthread_mutex_unlock(&colaExecMutex);
						break;
					} else {
						log_info(kernel_logger, "Hay recursos para %s - Instancias: %d", r->nombre, r->instancias);
						r->instancias--;
						// Poner recurso de la lista de recurso tomados
						t_list* rec_tom_wait = list_get(recursos_de_procesos, pcb_wait->pid);
						list_add(rec_tom_wait, r);

						// No hay recursos necesarias para que siga ejecutando
						send_recursos_ok(fd_cpu_dispatch, 1);
						break;
					}
				}
			}

			free(pcb_wait->registros);
			free(pcb_wait);
			free(recurso_wait_recibido);
			free(recurso_wait);
			break;
		case SIGNAL:
			t_pcb* pcb_signal = malloc(sizeof(t_pcb));
			pcb_signal->registros = malloc(sizeof(t_registros));
			char* recurso_signal = malloc(MAX_LENGTH);
			char* recurso_signal_recibido = malloc(MAX_LENGTH);
			t_recurso* r;

			if(!recv_wait_signal(fd_cpu_dispatch, pcb_signal, recurso_signal_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir el SIGNAL");
			}
			strcpy(recurso_signal, recurso_signal_recibido);

			for(int i = 0; i < list_size(recursos); i++) {
				r = list_get(recursos,i);
				if(strcmp(recurso_signal, r->nombre) == 0) {
					r->instancias++;
				}
			}

			// Conseguir recurso de la lista de recurso tomados
			t_list* rec_tom_signal = list_get(recursos_de_procesos, pcb_signal->pid);

			for(int i = 0; i < list_size(rec_tom_signal); i++) {
				t_recurso* r = list_get(rec_tom_signal,i);
				if(strcmp(recurso_signal, r->nombre) == 0) {
					list_remove(rec_tom_signal, i);
					break;
				}
			}

			// Revisar si otro proceso se puede desbloquear
			if(!queue_is_empty(r->blocked)) {
				t_pcb* pcb_desbloqueado = queue_pop(r->blocked);
				pthread_mutex_lock(&colaReadyMutex);
				queue_push(colaReady, pcb_desbloqueado);
				pthread_mutex_unlock(&colaReadyMutex);
			}


			free(pcb_signal->registros);
			free(pcb_signal);
			free(recurso_signal);
			free(recurso_signal_recibido);
			break;
		case IO_GEN_SLEEP:
			pthread_mutex_lock(&mutexIO);
			// pausar tiempo --> temporal_stop(time_vrr);
			if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
				temporal_stop(tiempo_vrr);
			}
			t_pcb* pcb_io_gen_sleep = malloc(sizeof(t_pcb));
			pcb_io_gen_sleep->registros = malloc(sizeof(t_registros));
			uint32_t unidades_de_trabajo;
			char* nombre_recibido_sleep = malloc(MAX_LENGTH);

			if(!recv_io_gen_sleep(fd_cpu_dispatch, pcb_io_gen_sleep, &unidades_de_trabajo, nombre_recibido_sleep)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_GEN_SLEEP");
			}
			// No es necesario de utilizar --> Esta asi para que la IO sepa que proceso interrumpir
			free(pcb_io_gen_sleep->registros);
			free(pcb_io_gen_sleep);
			strcpy(nombre_interfaz, nombre_recibido_sleep);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_interfaz);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "Iniciando interrupcion del proceso %d", pcb_recibido->pid);
				if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0) {
					log_info(kernel_logger, "Se recibio una IO antes del Quantum");
					pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
				} else if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
					log_info(kernel_logger, "Se recibio una IO antes del Quantum");
					pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
					// Resto el tiempo tomado de time.h con el pcb_recibido->quantum
					uint32_t tiempo_restante = temporal_gettime(tiempo_vrr);
					pcb_recibido->quantum -= tiempo_restante;
					log_info(kernel_logger, "Tiempo restante de quantum: %d", tiempo_restante);

					temporal_destroy(tiempo_vrr);

					if (pcb_recibido->quantum <= 0) {
					 	pcb_recibido->quantum = QUANTUM;
					}
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

            free(nombre_recibido_sleep);
			break;
		case IO_STDIN_READ:
			pthread_mutex_lock(&mutexIO);
			// pausar tiempo --> temporal_stop(time_vrr);
			if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
				temporal_stop(tiempo_vrr);
			}
			t_pcb* pcb_io_stdin = malloc(sizeof(t_pcb));
			pcb_io_stdin->registros = malloc(sizeof(t_registros));
			uint32_t direccion_fisica_stdin;
			uint32_t tamanio_maximo_stdin;
			char* nombre_recibido_stdin = malloc(MAX_LENGTH);

			if(!recv_io_stdin_stdout(fd_cpu_dispatch, pcb_io_stdin, &direccion_fisica_stdin, &tamanio_maximo_stdin, nombre_recibido_stdin)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_STDIN_READ");
			}
			// No es necesario de utilizar --> Esta asi para que la IO sepa que proceso interrumpir
			free(pcb_io_stdin->registros);
			free(pcb_io_stdin);
			strcpy(nombre_interfaz, nombre_recibido_stdin);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_interfaz);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "Iniciando interrupcion del proceso %d", pcb_recibido->pid);
				if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0) {
					log_info(kernel_logger, "Se recibio una IO antes del Quantum");
					pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
				} else if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
					log_info(kernel_logger, "Se recibio una IO antes del Quantum");
					pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
					// Resto el tiempo tomado de time.h con el pcb_recibido->quantum
					uint32_t tiempo_restante = temporal_gettime(tiempo_vrr);
					pcb_recibido->quantum -= tiempo_restante;
					log_info(kernel_logger,"Tiempo restante de quantum: %d",tiempo_restante);

					temporal_destroy(tiempo_vrr);

					if (pcb_recibido->quantum <= 0)
					{
					 	pcb_recibido->quantum = QUANTUM;
					}
				}
				
				pthread_mutex_lock(&colaBlockedMutex);
				pcb_recibido->estado = 'B';
				queue_push(colaBlocked, pcb_recibido);
				pthread_mutex_unlock(&colaBlockedMutex);
				// Envia PCB y lo necesario para la IO
				send_io_stdin_read(fd_interfaz, pcb_recibido, direccion_fisica_stdin, tamanio_maximo_stdin, nombre_interfaz, strlen(nombre_interfaz) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);
			pthread_mutex_unlock(&mutexIO);

            free(nombre_recibido_stdin);
			break;
		case IO_STDOUT_WRITE:
			pthread_mutex_lock(&mutexIO);
			// pausar tiempo --> temporal_stop(time_vrr);
			if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
				temporal_stop(tiempo_vrr);
			}
			t_pcb* pcb_io_stdout = malloc(sizeof(t_pcb));
			pcb_io_stdout->registros = malloc(sizeof(t_registros));
			uint32_t direccion_fisica_stdout;
			uint32_t tamanio_maximo_stdout;
			char* nombre_recibido = malloc(MAX_LENGTH);

			if(!recv_io_stdin_stdout(fd_cpu_dispatch, pcb_io_stdout, &direccion_fisica_stdout, &tamanio_maximo_stdout, nombre_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_STDIN_READ");
			}
			// No es necesario de utilizar --> Esta asi para que la IO sepa que proceso interrumpir
			free(pcb_io_stdout->registros);
			free(pcb_io_stdout);
			strcpy(nombre_interfaz, nombre_recibido);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_interfaz);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "Iniciando interrupcion del proceso %d", pcb_recibido->pid);
				if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0) {
					log_info(kernel_logger, "Se recibio una IO antes del Quantum");
					pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
				} else if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
					log_info(kernel_logger, "Se recibio una IO antes del Quantum");
					pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
					// Resto el tiempo tomado de time.h con el pcb_recibido->quantum
					uint32_t tiempo_restante = temporal_gettime(tiempo_vrr);
					pcb_recibido->quantum -= tiempo_restante;
					log_info(kernel_logger,"Tiempo restante de quantum: %d",tiempo_restante);

					temporal_destroy(tiempo_vrr);

					if (pcb_recibido->quantum <= 0)
					{
					 	pcb_recibido->quantum = QUANTUM;
					}
				}
				
				pthread_mutex_lock(&colaBlockedMutex);
				pcb_recibido->estado = 'B';
				queue_push(colaBlocked, pcb_recibido);
				pthread_mutex_unlock(&colaBlockedMutex);
				// Envia PCB y lo necesario para la IO
				send_io_stdout_write(fd_interfaz, pcb_recibido, direccion_fisica_stdout, tamanio_maximo_stdout, nombre_interfaz, strlen(nombre_interfaz) + 1);
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