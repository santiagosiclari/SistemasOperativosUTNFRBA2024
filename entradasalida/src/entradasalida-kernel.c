#include "../include/entradasalida-kernel.h"

void conexion_entradasalida_kernel() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case IO_GEN_SLEEP:
			uint32_t MAX_LENGTH = 128;
			uint32_t unidades_de_trabajo;
			char* nombre_interfaz = malloc(MAX_LENGTH);
			char* nombre_recivido = malloc(MAX_LENGTH);
			if(!recv_io_gen_sleep(fd_kernel, &unidades_de_trabajo, nombre_recivido)) {
				log_error(entradasalida_logger, "Hubo un error al recibir la interfaz IO_GEN_SLEEP");
			}
			strcpy(nombre_interfaz, nombre_recivido);

			if(strcmp(nombre_interfaz, nombre) != 0) {
				log_info(entradasalida_logger, "El nombre recivido con coincide con la interfaz. Puede estar ejecutandose en otro lado.");
				break;
			}

			uint32_t usleep_final = TIEMPO_UNIDAD_TRABAJO * unidades_de_trabajo;
			
			log_info(entradasalida_logger, "Realizando el usleep de %d", usleep_final);
			// Hace el sleep
			usleep(usleep_final);
			// Avisa que ya no esta mas interrumpido el proceso
			send_fin_io(fd_kernel, nombre, strlen(nombre) + 1);

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