#include "../include/cpu-memoria.h"

#define MAX_LENGTH 128

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
		    // Usar semaforos 
			// Ciclo de instruccion
			t_pcb* pcb;
			char* instruccion = malloc(MAX_LENGTH);
			char** instruccion_separada;
			// Fetch
			// Pedir instruccion
			if(!send_pc(fd_memoria, pcb->pc)) {
				log_error(cpu_logger, "Hubo un error al enviar el PC (Program Counter) al modulo de Memoria");
			} else {
				log_info(cpu_logger, "Se envio el PC %d a memoria", pcb->pc);
			}
				
			// Recibir la instruccion
			if (!recv_instruccion(fd_memoria, instruccion, MAX_LENGTH)) {
				log_error(cpu_logger, "Hubo un error al recibir la instruccion del modulo de Memoria");
			}

			log_info(cpu_logger, "Iniciando instruccion: %d", pcb->pc);

			// Decode
			instruccion_separada = string_split(instruccion, " ");
			if (strcmp(instruccion_separada[0], "SET") == 0) {
				pcb = cpu_set(pcb, instruccion_separada);
			} else if (strcmp(instruccion_separada[0], "SUM") == 0) {
				pcb = cpu_sum(pcb, instruccion_separada);
			} else if (strcmp(instruccion_separada[0], "SUB") == 0) {
				pcb = cpu_sub(pcb, instruccion_separada);
			} else if (strcmp(instruccion_separada[0], "JNZ") == 0) {
				pcb = cpu_jnz(pcb, instruccion_separada);
			} else if (strcmp(instruccion_separada[0], "IO_GEN_SLEEP") == 0) {
				pcb = cpu_io_gen_sleep(pcb, instruccion_separada);
			} else {
				log_warning(cpu_logger, "No se pudo encontrar la instruccion\n");
			}

			free(instruccion);
			free(instruccion_separada);
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