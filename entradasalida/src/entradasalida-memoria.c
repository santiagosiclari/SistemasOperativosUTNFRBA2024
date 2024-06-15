#include "../include/entradasalida-memoria.h"

void conexion_entradasalida_memoria() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case RECIBIR_VALOR_MEMORIA:
			// Cambiar el length de uint8 a uint32 !!
			void* datos;
			uint32_t length;
			uint32_t dir_fisica;
			if(!recv_valor_memoria(fd_memoria, &dir_fisica, &datos, &length)) {
				log_error(entradasalida_logger, "Hubo un error al recibir los datos de memoria");
			}
			char* datos_char = malloc(length + 1);
			memcpy(datos_char, datos, length);

			// Muestra el string en pantalla
			log_info(entradasalida_logger, "%s", datos_char);

			// Avisa que ya no esta mas interrumpido el proceso
			// uint32_t length_stdout = strlen(nombre_stdout) + 1;
			send_fin_io(fd_kernel, pcb_stdout->pid, nombre_stdout, strlen(nombre_stdout) + 1);

			// Liberar memoria
			free(pcb_stdout->registros);
			free(pcb_stdout);
			free(nombre_stdout);
			free(nombre_stdout_recibido);
			free(datos_char);
			free(datos);

			pthread_mutex_unlock(&mutexIO);
			break;
		case ESCRITURA_OK:
			uint8_t escritura_ok;
			
			if(!recv_escritura_ok(fd_memoria, &escritura_ok)) {
				log_error(entradasalida_logger, "Error al recibir escritura OK");
			}

			if(escritura_ok == 1) {
				log_info(entradasalida_logger, "Escritura OK");
			} else {
				log_error(entradasalida_logger, "Error en la escritura");
			}

			// Avisa que ya no esta mas interrumpido el proceso
			// uint32_t length_stdin = strlen(nombre_stdin) + 1;
			send_fin_io(fd_kernel, pcb_stdin->pid, nombre_stdin, strlen(nombre_stdin) + 1);

			// Liberar memoria
			free(pcb_stdin->registros);
			free(pcb_stdin);
			free(nombre_stdin);
			free(nombre_stdin_recibido);
			free(string);

			pthread_mutex_unlock(&mutexIO);
			break;
		case -1:
			log_error(entradasalida_logger, "El servidor de Memoria no se encuentra activo.");
			control = 0;
            break;
		default:
			log_warning(entradasalida_logger, "Operacion desconocida.");
			break;
		}
	}
}