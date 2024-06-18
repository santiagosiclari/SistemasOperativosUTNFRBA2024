#include "../include/kernel-cpu-dispatch.h"

int fd_interfaz;
char* nombre_sleep;
char* nombre_recibido_sleep;
char* nombre_stdin;
char* nombre_recibido_stdin;
char* nombre_stdout;
char* nombre_recibido_stdout;
char* nombre_archivo;
char* nombre_archivo_recibido;
char* nombre_fs;
char* nombre_fs_recibido;

pthread_mutex_t mutexIO = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexFinQuantum = PTHREAD_MUTEX_INITIALIZER;

void conexion_kernel_cpu_dispatch() {
	uint8_t MAX_LENGTH = 128;
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_cpu_dispatch);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case RECIBIR_PID_A_BORRAR:
			if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0 || strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
				pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando se tiene que borrar el proceso
			}
			// Recibo proceso a eliminar
    		pthread_mutex_lock(&colaExecMutex);
			t_pcb* pcb_a_borrar;

			if(!queue_is_empty(colaExec)) {
				pcb_a_borrar = queue_pop(colaExec);

				if ((queue_size(colaNew) + size_all_queues()) == 0) {
					control_planificacion = 0;
				}
			}

			uint8_t pid_a_borrar;
			if(!recv_pid_a_borrar(fd_cpu_dispatch, &pid_a_borrar)) {
				log_error(kernel_logger, "Hubo un error al recibir el PID.");
			}

			// Revisar si otro proceso se puede desbloquear
			liberar_recursos(pid_a_borrar);
			send_fin_proceso(fd_memoria, pid_a_borrar);
            log_info(kernel_logger, "Finaliza el proceso %d - Motivo: %s", pcb_a_borrar->pid, "Success");

			free(pcb_a_borrar->registros);
			free(pcb_a_borrar);
			pthread_mutex_unlock(&colaExecMutex);
			break;
		case RECIBIR_PCB:
			// PCB interrumpido por fin de quantum
			pthread_mutex_lock(&mutexFinQuantum);
			t_pcb* pcb_interrumpido = malloc(sizeof(t_pcb));
			pcb_interrumpido->registros = malloc(sizeof(t_pcb));
			t_pcb* pcb_int;

			if(!recv_pcb(fd_cpu_dispatch, pcb_interrumpido)) {
				log_error(kernel_logger, "Hubo un error al recibir el PCB interrumpido");
				break;
			}
			temporal_destroy(tiempo_vrr);

			pthread_mutex_lock(&colaExecMutex);
			if(queue_size(colaExec) != 0) {
				pcb_int = queue_pop(colaExec);
				if(pcb_int->pid != pcb_interrumpido->pid) {
					free(pcb_interrumpido->registros);
					free(pcb_interrumpido);
					pthread_mutex_unlock(&colaExecMutex);
					pthread_mutex_unlock(&mutexFinQuantum);
					break;
				}
				log_info(kernel_logger, "PID: %d - Desalojado por fin de Quantum", pcb_int->pid);
			}
			pthread_mutex_unlock(&colaExecMutex);

			pthread_mutex_lock(&colaReadyMutex);
			pcb_int->estado = 'R';
			queue_push(colaReady, pcb_int);
			pthread_mutex_unlock(&colaReadyMutex);
			pthread_mutex_unlock(&mutexFinQuantum);
			free(pcb_interrumpido->registros);
			free(pcb_interrumpido);
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
					if(r->instancias <= 0) {
						log_info(kernel_logger, "PID: %d - Bloqueado por: %s", pcb_wait->pid, r->nombre);
						// El proceso se bloquea en la cola correspondiente dependiendo el recurso tomado
           				pthread_mutex_lock(&colaExecMutex);
						if(!queue_is_empty(colaExec)) {
							// No hay recursos necesarias para que siga ejecutando
							send_recursos_ok(fd_cpu_dispatch, -1);
							
							t_pcb* pcb_recv = queue_pop(colaExec);
							if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0) {
								pthread_cancel(quantum_thread); // Cancelar el hilo del quantum cuando recibe una IO
							} else if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
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
			string_trim_right(&recurso_signal);

			for(int i = 0; i < list_size(recursos); i++) {
				r = list_get(recursos, i);
				if(strcmp(recurso_signal, r->nombre) == 0) {
					r->instancias++;
				}
			}

			// Conseguir recurso de la lista de recurso tomados
			t_list* rec_tom_signal = list_get(recursos_de_procesos, pcb_signal->pid);

			for(int i = 0; i < list_size(rec_tom_signal); i++) {
				r = list_get(rec_tom_signal,i);
				if(strcmp(recurso_signal, r->nombre) == 0) {
					list_remove(rec_tom_signal, i);
					break;
				}
			}

			if(!queue_is_empty(r->blocked)) {
				t_pcb* pcb_desbloqueado = queue_pop(r->blocked);
				pthread_mutex_lock(&colaReadyMutex);
				queue_push(colaReady, pcb_desbloqueado);
				pthread_mutex_unlock(&colaReadyMutex);
				log_info(kernel_logger, "Proceso %d desbloqueado y enviado a Ready", pcb_desbloqueado->pid);
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
			nombre_sleep = malloc(MAX_LENGTH);
			nombre_recibido_sleep = malloc(MAX_LENGTH);

			if(!recv_io_gen_sleep(fd_cpu_dispatch, pcb_io_gen_sleep, &unidades_de_trabajo, nombre_recibido_sleep)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_GEN_SLEEP");
			}
			strcpy(nombre_sleep, nombre_recibido_sleep);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_sleep);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "PID: %d - Bloqueado por: %s", pcb_recibido->pid, nombre_sleep);
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
				send_io_gen_sleep(fd_interfaz, pcb_recibido, unidades_de_trabajo, nombre_sleep, strlen(nombre_sleep) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);
			pthread_mutex_unlock(&mutexIO);

			free(pcb_io_gen_sleep->registros);
			free(pcb_io_gen_sleep);
            free(nombre_sleep);
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
			nombre_stdin = malloc(MAX_LENGTH);
			nombre_recibido_stdin = malloc(MAX_LENGTH);

			if(!recv_io_stdin_stdout(fd_cpu_dispatch, pcb_io_stdin, &direccion_fisica_stdin, &tamanio_maximo_stdin, nombre_recibido_stdin)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_STDIN_READ");
			}
			strcpy(nombre_stdin, nombre_recibido_stdin);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_stdin);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "PID: %d - Bloqueado por: %s", pcb_recibido->pid, nombre_stdin);
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
				send_io_stdin_read(fd_interfaz, pcb_recibido, direccion_fisica_stdin, tamanio_maximo_stdin, nombre_stdin, strlen(nombre_stdin) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);
			pthread_mutex_unlock(&mutexIO);

			free(pcb_io_stdin->registros);
			free(pcb_io_stdin);
            free(nombre_stdin);
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
			nombre_stdout = malloc(MAX_LENGTH);
			nombre_recibido_stdout = malloc(MAX_LENGTH);

			if(!recv_io_stdin_stdout(fd_cpu_dispatch, pcb_io_stdout, &direccion_fisica_stdout, &tamanio_maximo_stdout, nombre_recibido_stdout)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_STDIN_READ");
			}
			strcpy(nombre_stdout, nombre_recibido_stdout);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_stdout);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "PID: %d - Bloqueado por: %s", pcb_recibido->pid, nombre_stdout);
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
				send_io_stdout_write(fd_interfaz, pcb_recibido, direccion_fisica_stdout, tamanio_maximo_stdout, nombre_stdout, strlen(nombre_stdout) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);
			pthread_mutex_unlock(&mutexIO);

			free(pcb_io_stdout->registros);
			free(pcb_io_stdout);
            free(nombre_stdout);
			free(nombre_recibido_stdout);
			break;
		case IO_FS_CREATE:
			pthread_mutex_lock(&mutexIO);
			// pausar tiempo --> temporal_stop(time_vrr);
			if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
				temporal_stop(tiempo_vrr);
			}
			t_pcb* pcb_io_fs_create = malloc(sizeof(t_pcb));
			pcb_io_fs_create->registros = malloc(sizeof(t_registros));
			nombre_archivo = malloc(MAX_LENGTH);
			nombre_archivo_recibido = malloc(MAX_LENGTH);
			nombre_fs = malloc(MAX_LENGTH);
			nombre_fs_recibido = malloc(MAX_LENGTH);

			if(!recv_io_fs_create_delete(fd_cpu_dispatch, pcb_io_fs_create, nombre_archivo_recibido, nombre_fs_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_FS_CREATE");
			}
			strcpy(nombre_archivo, nombre_archivo_recibido);
			strcpy(nombre_fs, nombre_fs_recibido);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_fs);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "PID: %d - Bloqueado por: %s", pcb_recibido->pid, nombre_fs);
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
				send_io_fs_create(fd_interfaz, pcb_recibido, nombre_archivo, strlen(nombre_archivo) + 1, nombre_fs, strlen(nombre_fs) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);
			pthread_mutex_unlock(&mutexIO);

			free(pcb_io_fs_create->registros);
			free(pcb_io_fs_create);
            free(nombre_archivo);
			free(nombre_archivo_recibido);
            free(nombre_fs);
			free(nombre_fs_recibido);
			break;
		case IO_FS_DELETE:
			pthread_mutex_lock(&mutexIO);
			// pausar tiempo --> temporal_stop(time_vrr);
			if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
				temporal_stop(tiempo_vrr);
			}
			t_pcb* pcb_io_fs_delete = malloc(sizeof(t_pcb));
			pcb_io_fs_delete->registros = malloc(sizeof(t_registros));
			nombre_archivo = malloc(MAX_LENGTH);
			nombre_archivo_recibido = malloc(MAX_LENGTH);
			nombre_fs = malloc(MAX_LENGTH);
			nombre_fs_recibido = malloc(MAX_LENGTH);

			if(!recv_io_fs_create_delete(fd_cpu_dispatch, pcb_io_fs_delete, nombre_archivo_recibido, nombre_fs_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_FS_DELETE");
			}
			strcpy(nombre_archivo, nombre_archivo_recibido);
			strcpy(nombre_fs, nombre_fs_recibido);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_fs);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "PID: %d - Bloqueado por: %s", pcb_recibido->pid, nombre_fs);
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
				send_io_fs_delete(fd_interfaz, pcb_recibido, nombre_archivo, strlen(nombre_archivo) + 1, nombre_fs, strlen(nombre_fs) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);
			pthread_mutex_unlock(&mutexIO);

			free(pcb_io_fs_delete->registros);
			free(pcb_io_fs_delete);
            free(nombre_archivo);
			free(nombre_archivo_recibido);
            free(nombre_fs);
			free(nombre_fs_recibido);
			break;
		case IO_FS_TRUNCATE:
			pthread_mutex_lock(&mutexIO);
			// pausar tiempo --> temporal_stop(time_vrr);
			if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
				temporal_stop(tiempo_vrr);
			}
			t_pcb* pcb_io_fs_truncate = malloc(sizeof(t_pcb));
			pcb_io_fs_truncate->registros = malloc(sizeof(t_registros));
			uint32_t tamanio_truncate;
			nombre_archivo = malloc(MAX_LENGTH);
			nombre_archivo_recibido = malloc(MAX_LENGTH);
			nombre_fs = malloc(MAX_LENGTH);
			nombre_fs_recibido = malloc(MAX_LENGTH);

			if(!recv_io_fs_truncate(fd_cpu_dispatch, pcb_io_fs_truncate, &tamanio_truncate, nombre_archivo_recibido, nombre_fs_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_FS_TRUNCATE");
			}
			strcpy(nombre_archivo, nombre_archivo_recibido);
			strcpy(nombre_fs, nombre_fs_recibido);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_fs);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "PID: %d - Bloqueado por: %s", pcb_recibido->pid, nombre_fs);
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
				send_io_fs_truncate(fd_interfaz, pcb_recibido, tamanio_truncate, nombre_archivo, strlen(nombre_archivo) + 1, nombre_fs, strlen(nombre_fs) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);
			pthread_mutex_unlock(&mutexIO);

			free(pcb_io_fs_truncate->registros);
			free(pcb_io_fs_truncate);
            free(nombre_archivo);
			free(nombre_archivo_recibido);
            free(nombre_fs);
			free(nombre_fs_recibido);
			break;
		case IO_FS_WRITE:
			pthread_mutex_lock(&mutexIO);
			// pausar tiempo --> temporal_stop(time_vrr);
			if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
				temporal_stop(tiempo_vrr);
			}
			t_pcb* pcb_io_fs_write = malloc(sizeof(t_pcb));
			pcb_io_fs_write->registros = malloc(sizeof(t_registros));
			uint32_t tamanio_write, dir_fisica_write, ptr_archivo_write;
			nombre_archivo = malloc(MAX_LENGTH);
			nombre_archivo_recibido = malloc(MAX_LENGTH);
			nombre_fs = malloc(MAX_LENGTH);
			nombre_fs_recibido = malloc(MAX_LENGTH);

			if(!recv_io_fs_write_read(fd_cpu_dispatch, pcb_io_fs_write, &tamanio_write, &dir_fisica_write, &ptr_archivo_write, nombre_archivo_recibido, nombre_fs_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_FS_WRITE");
			}
			strcpy(nombre_archivo, nombre_archivo_recibido);
			strcpy(nombre_fs, nombre_fs_recibido);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_fs);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "PID: %d - Bloqueado por: %s", pcb_recibido->pid, nombre_fs);
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
				send_io_fs_write(fd_interfaz, pcb_recibido, tamanio_write, dir_fisica_write, ptr_archivo_write, nombre_archivo, strlen(nombre_archivo) + 1, nombre_fs, strlen(nombre_fs) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);
			pthread_mutex_unlock(&mutexIO);

			free(pcb_io_fs_write->registros);
			free(pcb_io_fs_write);
            free(nombre_archivo);
			free(nombre_archivo_recibido);
            free(nombre_fs);
			free(nombre_fs_recibido);
			break;
		case IO_FS_READ:
			pthread_mutex_lock(&mutexIO);
			// pausar tiempo --> temporal_stop(time_vrr);
			if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
				temporal_stop(tiempo_vrr);
			}
			t_pcb* pcb_io_fs_read = malloc(sizeof(t_pcb));
			pcb_io_fs_read->registros = malloc(sizeof(t_registros));
			uint32_t tamanio_read, dir_fisica_read, ptr_archivo_read;
			nombre_archivo = malloc(MAX_LENGTH);
			nombre_archivo_recibido = malloc(MAX_LENGTH);
			nombre_fs = malloc(MAX_LENGTH);
			nombre_fs_recibido = malloc(MAX_LENGTH);

			if(!recv_io_fs_write_read(fd_cpu_dispatch, pcb_io_fs_read, &tamanio_read, &dir_fisica_read, &ptr_archivo_read, nombre_archivo_recibido, nombre_fs_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir la interfaz IO_FS_READ");
			}
			strcpy(nombre_archivo, nombre_archivo_recibido);
			strcpy(nombre_fs, nombre_fs_recibido);
			
			// Busca el socket de la interfaz
			fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_fs);

            pthread_mutex_lock(&colaExecMutex);
			if(!queue_is_empty(colaExec)) {
				t_pcb* pcb_recibido = queue_pop(colaExec);
				log_info(kernel_logger, "PID: %d - Bloqueado por: %s", pcb_recibido->pid, nombre_fs);
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
				send_io_fs_read(fd_interfaz, pcb_recibido, tamanio_read, dir_fisica_read, ptr_archivo_read, nombre_archivo, strlen(nombre_archivo) + 1, nombre_fs, strlen(nombre_fs) + 1);
			}
			pthread_mutex_unlock(&colaExecMutex);
			pthread_mutex_unlock(&mutexIO);

			free(pcb_io_fs_read->registros);
			free(pcb_io_fs_read);
            free(nombre_archivo);
			free(nombre_archivo_recibido);
            free(nombre_fs);
			free(nombre_fs_recibido);
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