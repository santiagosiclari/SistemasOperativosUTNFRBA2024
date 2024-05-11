#include "../include/cpu-memoria.h"

#define MAX_LENGTH 128

uint8_t size_instrucciones;

void conexion_cpu_memoria() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case RECIBIR_INSTRUCCION:
			// t_pcb* pcb = malloc(sizeof(t_pcb));
			char* instruccion = malloc(MAX_LENGTH);
			char** instruccion_separada;

			// Recibir la instruccion
			if (!recv_instruccion(fd_memoria, instruccion)) {
				log_error(cpu_logger, "Hubo un error al recibir la instruccion del modulo de Memoria");
			}

			// log_info(cpu_logger, "Iniciando instruccion: %d", pcb->pc);

			// Decode
			instruccion_separada = string_split(instruccion, " ");
			if (strcmp(instruccion_separada[0], "SET") == 0) {
				// set(pcb, instruccion_separada);
			} else if (strcmp(instruccion_separada[0], "SUM") == 0) {
				// sum(pcb, instruccion_separada);
			} else if (strcmp(instruccion_separada[0], "SUB") == 0) {
				// sub(pcb, instruccion_separada);
			} else if (strcmp(instruccion_separada[0], "JNZ") == 0) {
				// jnz(pcb, instruccion_separada);
			} else if (strcmp(instruccion_separada[0], "IO_GEN_SLEEP") == 0) {
				// io_gen_sleep(pcb, instruccion_separada);
			} else {
				log_warning(cpu_logger, "No se pudo encontrar la instruccion\n");
			}

			free(instruccion);
			// free(instruccion_separada);
			break;
		case RECIBIR_SIZE_INSTRUCCIONES:
			if(!recv_size_instrucciones(fd_memoria, size_instrucciones)) {
				log_error(cpu_logger, "Hubo un error al recibir la cantidad de instrucciones a ejecutar");
			} else {
				log_info(cpu_logger, "Cantidad de instrucciones a ejecutar %d", size_instrucciones);
			}
			break;
		case -1:
			log_error(cpu_logger, "El servidor de Memoria no se encuentra activo.");
			control = 0;
            break;
		default:
			log_warning(cpu_logger, "Operacion desconocida.");
			break;
		}
	}
}