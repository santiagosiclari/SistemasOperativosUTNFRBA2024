#include "../include/entradasalida-kernel.h"

char* nombre_interfaz;
char* nombre_recibido;
uint32_t MAX_LENGTH;

void conexion_entradasalida_kernel() {
	MAX_LENGTH = 128;
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case IO_GEN_SLEEP:
			t_pcb* pcb_sleep = malloc(sizeof(t_pcb));
			pcb_sleep->registros = malloc(sizeof(t_registros));
			uint32_t unidades_de_trabajo;
			nombre_interfaz = malloc(MAX_LENGTH);
			nombre_recibido = malloc(MAX_LENGTH);

			if(!recv_io_gen_sleep(fd_kernel, pcb_sleep, &unidades_de_trabajo, nombre_recibido)) {
				log_error(entradasalida_logger, "Hubo un error al recibir la interfaz IO_GEN_SLEEP");
			}
			strcpy(nombre_interfaz, nombre_recibido);

			if(strcmp(nombre_interfaz, nombre) != 0) {
				log_info(entradasalida_logger, "El nombre recibido no coincide con la interfaz.");
				send_fin_io(fd_kernel, nombre, strlen(nombre) + 1);
				free(pcb_sleep->registros);
				free(pcb_sleep);
				free(nombre_interfaz);
				free(nombre_recibido);
				break;
			}

			uint32_t usleep_final = TIEMPO_UNIDAD_TRABAJO * unidades_de_trabajo;
			
			log_info(entradasalida_logger, "Realizando el usleep de %d - PID: %d", usleep_final, pcb_sleep->pid);
			// Hace el sleep
			usleep(usleep_final);
			log_info(entradasalida_logger, "Finalizo el usleep de %d - PID: %d", usleep_final, pcb_sleep->pid);

			// Avisa que ya no esta mas interrumpido el proceso
			send_fin_io(fd_kernel, nombre, strlen(nombre) + 1);

			// Liberar memoria
			free(pcb_sleep->registros);
			free(pcb_sleep);
			free(nombre_interfaz);
			free(nombre_recibido);
			break;
		case IO_STDIN_READ:
			t_pcb* pcb_stdin = malloc(sizeof(t_pcb));
			pcb_stdin->registros = malloc(sizeof(t_registros));
			nombre_interfaz = malloc(MAX_LENGTH);
			nombre_recibido = malloc(MAX_LENGTH);
			uint32_t direccion_fisica_stdin;
			uint32_t tamanio_maximo_stdin;

			if(!recv_io_stdin_stdout(fd_kernel, pcb_stdin, &direccion_fisica_stdin, &tamanio_maximo_stdin, nombre_recibido)) {
				log_error(entradasalida_logger, "Hubo un error al recibir la interfaz IO_STDIN_READ");
			}
			strcpy(nombre_interfaz, nombre_recibido);

			if(strcmp(nombre_interfaz, nombre) != 0) {
				log_info(entradasalida_logger, "El nombre recibido no coincide con la interfaz.");
				send_fin_io(fd_kernel, nombre, strlen(nombre) + 1);
				free(pcb_stdin->registros);
				free(pcb_stdin);
				free(nombre_interfaz);
				free(nombre_recibido);
				break;
			}
			
			// Pedir string
			char* string = malloc(tamanio_maximo_stdin + 1);
			string = readline("> ");
			if (strlen(string) > tamanio_maximo_stdin) {
				log_info(entradasalida_logger, "El string supera los valores maximos.");
				send_fin_io(fd_kernel, nombre, strlen(nombre) + 1);
				free(pcb_stdin->registros);
				free(pcb_stdin);
				free(nombre_interfaz);
				free(nombre_recibido);
				break;
			}

			// Escribir en memoria (recibir escritura_ok)
			send_escribir_memoria(fd_memoria, pcb_stdin->pid, direccion_fisica_stdin, string, strlen(string) + 1);

			// Liberar memoria
			free(string);
			free(pcb_stdin->registros);
			free(pcb_stdin);
			free(nombre_interfaz);
			free(nombre_recibido);
			break;
		case IO_STDOUT_WRITE:
			t_pcb* pcb_stdout = malloc(sizeof(t_pcb));
			pcb_stdout->registros = malloc(sizeof(t_registros));
			nombre_interfaz = malloc(MAX_LENGTH);
			nombre_recibido = malloc(MAX_LENGTH);
			uint32_t direccion_fisica_stdout;
			uint32_t tamanio_maximo_stdout;

			if(!recv_io_stdin_stdout(fd_kernel, pcb_stdout, &direccion_fisica_stdout, &tamanio_maximo_stdout, nombre_recibido)) {
				log_error(entradasalida_logger, "Hubo un error al recibir la interfaz IO_STDOUT_WRITE");
			}
			strcpy(nombre_interfaz, nombre_recibido);

			if(strcmp(nombre_interfaz, nombre) != 0) {
				log_info(entradasalida_logger, "El nombre recibido no coincide con la interfaz.");
				send_fin_io(fd_kernel, nombre, strlen(nombre) + 1);
				free(pcb_stdout->registros);
				free(pcb_stdout);
				free(nombre_interfaz);
				free(nombre_recibido);
				break;
			}

			// Logica de leer memoria y mostrar en pantalla el string (recibir_valor)
			send_leer_memoria(fd_memoria, pcb_stdout->pid, direccion_fisica_stdout, tamanio_maximo_stdout);

			// Avisa que ya no esta mas interrumpido el proceso
			send_fin_io(fd_kernel, nombre, strlen(nombre) + 1);

			// Liberar memoria
			free(pcb_stdout->registros);
			free(pcb_stdout);
			free(nombre_interfaz);
			free(nombre_recibido);
			break;
		case -1:
			log_error(entradasalida_logger, "El servidor de Kernel no se encuentra activo.");
			control = 0;
            break;
		default:
			log_warning(entradasalida_logger, "Operacion desconocida.");
			break;
		}
	}
}