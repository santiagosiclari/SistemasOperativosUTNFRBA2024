#include "../include/entradasalida-kernel.h"

t_pcb* pcb_stdout;
t_pcb* pcb_stdin;
char* nombre_sleep;
char* nombre_sleep_recibido;
char* nombre_stdin;
char* nombre_stdin_recibido;
char* nombre_stdout;
char* nombre_stdout_recibido;
char* string;
uint32_t MAX_LENGTH;
pthread_mutex_t mutexIO = PTHREAD_MUTEX_INITIALIZER;

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
			pthread_mutex_lock(&mutexIO);
			t_pcb* pcb_sleep = malloc(sizeof(t_pcb));
			pcb_sleep->registros = malloc(sizeof(t_registros));
			uint32_t unidades_de_trabajo;
			nombre_sleep = malloc(MAX_LENGTH);
			nombre_sleep_recibido = malloc(MAX_LENGTH);

			if(!recv_io_gen_sleep(fd_kernel, pcb_sleep, &unidades_de_trabajo, nombre_sleep_recibido)) {
				log_error(entradasalida_logger, "Hubo un error al recibir la interfaz IO_GEN_SLEEP");
			}
			strcpy(nombre_sleep, nombre_sleep_recibido);

			uint32_t usleep_final = TIEMPO_UNIDAD_TRABAJO * unidades_de_trabajo;
			
			log_info(entradasalida_logger, "PID: %d - Operacion: %s", pcb_sleep->pid, "IO_GEN_SLEEP");
			// Hace el sleep
			usleep(usleep_final);
			log_info(entradasalida_logger, "Finalizo el usleep de %d - PID: %d", usleep_final, pcb_sleep->pid);

			// Avisa que ya no esta mas interrumpido el proceso
			send_fin_io(fd_kernel, pcb_sleep->pid, nombre_sleep, strlen(nombre_sleep) + 1);

			// Liberar memoria
			free(pcb_sleep->registros);
			free(pcb_sleep);
			free(nombre_sleep);
			free(nombre_sleep_recibido);
			pthread_mutex_unlock(&mutexIO);
			break;
		case IO_STDIN_READ:
			pthread_mutex_lock(&mutexIO);
			pcb_stdin = malloc(sizeof(t_pcb));
			pcb_stdin->registros = malloc(sizeof(t_registros));
			nombre_stdin = malloc(MAX_LENGTH);
			nombre_stdin_recibido = malloc(MAX_LENGTH);
			uint32_t direccion_fisica_stdin;
			uint32_t tamanio_maximo_stdin;

			if(!recv_io_stdin_stdout(fd_kernel, pcb_stdin, &direccion_fisica_stdin, &tamanio_maximo_stdin, nombre_stdin_recibido)) {
				log_error(entradasalida_logger, "Hubo un error al recibir la interfaz IO_STDIN_READ");
			}
			strcpy(nombre_stdin, nombre_stdin_recibido);

			log_info(entradasalida_logger, "PID: %d - Operacion: %s", pcb_stdin->pid, "IO_STDIN_READ");
			log_info(entradasalida_logger, "Ingrese el string (maximo %d letras): ", tamanio_maximo_stdin);
			
			// Pedir string
			string = readline("> ");
			if (strlen(string) > tamanio_maximo_stdin) {
				log_info(entradasalida_logger, "El string supera los valores maximos.");
				send_fin_io(fd_kernel, pcb_stdin->pid, nombre_stdin, strlen(nombre_stdin) + 1);
				free(pcb_stdin->registros);
				free(pcb_stdin);
				free(nombre_stdin);
				free(nombre_stdin_recibido);
				break;
			}

			// Escribir en memoria (recibir escritura_ok)
			send_escribir_memoria(fd_memoria, pcb_stdin->pid, direccion_fisica_stdin, string, strlen(string) + 1);
			break;
		case IO_STDOUT_WRITE:
			pthread_mutex_lock(&mutexIO);
			pcb_stdout = malloc(sizeof(t_pcb));
			pcb_stdout->registros = malloc(sizeof(t_registros));
			nombre_stdout = malloc(MAX_LENGTH);
			nombre_stdout_recibido = malloc(MAX_LENGTH);
			uint32_t direccion_fisica_stdout;
			uint32_t tamanio_maximo_stdout;

			if(!recv_io_stdin_stdout(fd_kernel, pcb_stdout, &direccion_fisica_stdout, &tamanio_maximo_stdout, nombre_stdout_recibido)) {
				log_error(entradasalida_logger, "Hubo un error al recibir la interfaz IO_STDOUT_WRITE");
			}
			strcpy(nombre_stdout, nombre_stdout_recibido);

			log_info(entradasalida_logger, "PID: %d - Operacion: %s", pcb_stdout->pid, "IO_STDOUT_WRITE");

			// Logica de leer memoria y mostrar en pantalla el string (recibir_valor)
			send_leer_memoria(fd_memoria, pcb_stdout->pid, direccion_fisica_stdout, tamanio_maximo_stdout);
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