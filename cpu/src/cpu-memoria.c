#include "../include/cpu-memoria.h"

uint8_t size_instrucciones;

void crear_diccionario(t_dictionary* dictionary_registros) {
    dictionary_put(dictionary_registros, "AX", &pcb_a_ejecutar->registros->AX);
    dictionary_put(dictionary_registros, "BX", &pcb_a_ejecutar->registros->BX);
    dictionary_put(dictionary_registros, "CX", &pcb_a_ejecutar->registros->CX);
    dictionary_put(dictionary_registros, "DX", &pcb_a_ejecutar->registros->DX);
    dictionary_put(dictionary_registros, "EAX", &pcb_a_ejecutar->registros->EAX);
    dictionary_put(dictionary_registros, "EBX", &pcb_a_ejecutar->registros->EBX);
    dictionary_put(dictionary_registros, "ECX", &pcb_a_ejecutar->registros->ECX);
    dictionary_put(dictionary_registros, "EDX", &pcb_a_ejecutar->registros->EDX);
    dictionary_put(dictionary_registros, "SI", &pcb_a_ejecutar->registros->SI);
    dictionary_put(dictionary_registros, "DI", &pcb_a_ejecutar->registros->DI);
}

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
			int MAX_LENGTH = 128;
			char* instruccion = malloc(MAX_LENGTH);
			char* instruccion_recibida = malloc(MAX_LENGTH);
			char** instruccion_separada;
			t_dictionary* dictionary_registros = dictionary_create();
			crear_diccionario(dictionary_registros);

			if(pcb_a_ejecutar != NULL) {
				// Recibir la instruccion
				if (!recv_instruccion(fd_memoria, instruccion_recibida)) {
					log_error(cpu_logger, "Hubo un error al recibir la instruccion del modulo de Memoria");
					free(instruccion);
					free(instruccion_recibida);
					dictionary_destroy(dictionary_registros);
					break;
				}

				strcpy(instruccion, instruccion_recibida);

				log_info(cpu_logger, "Iniciando instruccion: %s - Nro: %d", instruccion, pcb_a_ejecutar->pc);

				// Decode
				instruccion_separada = string_split(instruccion, " ");
				if (strcmp(instruccion_separada[0], "SET") == 0) {
					int valor = atoi(instruccion_separada[2]);
					funcion_set(dictionary_registros, instruccion_separada[1], valor);
				} else if (strcmp(instruccion_separada[0], "SUM") == 0) {
					funcion_sum(dictionary_registros, instruccion_separada[1], instruccion_separada[2]);
				} else if (strcmp(instruccion_separada[0], "SUB") == 0) {
					funcion_sub(dictionary_registros, instruccion_separada[1], instruccion_separada[2]);
				} else if (strcmp(instruccion_separada[0], "JNZ") == 0) {
					uint32_t valor_pc = atoi(instruccion_separada[2]);
					funcion_jnz(dictionary_registros, instruccion_separada[1], valor_pc);
				} else if (strcmp(instruccion_separada[0], "IO_GEN_SLEEP") == 0) {
					uint32_t unidades_trabajo = atoi(instruccion_separada[2]);
					funcion_io_gen_sleep(instruccion_separada[1], unidades_trabajo);
				} else if (strcmp(instruccion_separada[0], "EXIT") == 0) {
					funcion_exit();
				} else {
					log_warning(cpu_logger, "No se pudo encontrar la instruccion: %s\n", instruccion_separada[0]);
				}

				// Ver si es EXIT
				if(pcb_a_ejecutar == NULL) {
					log_info(cpu_logger, "El proceso finalizo");
					free(instruccion);
					free(instruccion_recibida);
					for (int i = 0; instruccion_separada[i] != NULL; i++) {
						free(instruccion_separada[i]);
					}
					free(instruccion_separada);
					dictionary_destroy(dictionary_registros);
					break;
				}

				log_info(cpu_logger, "Instruccion finalizada");

				// Printea el PCB
				log_info(cpu_logger, "PID: %d\nProgram Counter: %d\nEstado: %c\nQuantum: %d\nFlag IO: %d\nRegistros:\nAX: %d, BX: %d, CX: %d, DX: %d\nEAX: %d, EBX: %d, ECX: %d, EDX: %d\nSI: %d, DI: %d",
				pcb_a_ejecutar->pid, pcb_a_ejecutar->pc, pcb_a_ejecutar->estado, pcb_a_ejecutar->quantum, pcb_a_ejecutar->flag_io,
				pcb_a_ejecutar->registros->AX, pcb_a_ejecutar->registros->BX, pcb_a_ejecutar->registros->CX, pcb_a_ejecutar->registros->DX,
				pcb_a_ejecutar->registros->EAX, pcb_a_ejecutar->registros->EBX, pcb_a_ejecutar->registros->ECX, pcb_a_ejecutar->registros->EDX,
				pcb_a_ejecutar->registros->SI, pcb_a_ejecutar->registros->DI);
				
				// Interrupcion por IO
				if(pcb_a_ejecutar->flag_io == 1) {
					log_info(cpu_logger, "Proceso %d fue interrumpido por una IO", pcb_a_ejecutar->pid);
					// send pcb a kernel --> contexto de ejecucion
					free(instruccion);
					free(instruccion_recibida);
					for (int i = 0; instruccion_separada[i] != NULL; i++) {
						free(instruccion_separada[i]);
					}
					free(instruccion_separada);
					dictionary_destroy(dictionary_registros);
					break;
				}

				// Interrupcion por fin de Quantum
				if(pcb_a_ejecutar->flag_io == 2) {
					log_info(cpu_logger, "Proceso %d finalizo su quantum", pcb_a_ejecutar->pid);
					// send pcb a kernel --> contexto de ejecucion
					free(instruccion);
					free(instruccion_recibida);
					for (int i = 0; instruccion_separada[i] != NULL; i++) {
						free(instruccion_separada[i]);
					}
					free(instruccion_separada);
					dictionary_destroy(dictionary_registros);
					break;
				}

				// Fetch --> seguir pidiendo instrucciones
				send_pc_pid(fd_memoria, pcb_a_ejecutar->pc, pcb_a_ejecutar->pid);
				log_info(cpu_logger, "Se envio el PC %d a memoria", pcb_a_ejecutar->pc);
			}

			// Libera la instruccion anterior
			free(instruccion);
			// Liberar el arreglo de la instrucción separada
			for (int i = 0; instruccion_separada[i] != NULL; i++) {
				free(instruccion_separada[i]);
			}
			free(instruccion_separada);
			dictionary_destroy(dictionary_registros);

			break;
		case RECIBIR_SIZE_INSTRUCCIONES:
			if(!recv_size_instrucciones(fd_memoria, &size_instrucciones)) {
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