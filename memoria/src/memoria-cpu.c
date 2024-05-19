#include "../include/memoria-cpu.h"

void conexion_memoria_cpu() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_cpu);
		switch (cod_op) {
		case MENSAJE:
			log_info(memoria_logger, "Mensaje recibido con el numero");
			break;
		case PAQUETE:
			break;
		case RECIBIR_PC_PID:
			uint8_t pid = 0;
			uint32_t pc = 0;
			if(!recv_pc_pid(fd_cpu, pc, pid)) { 
				log_error(memoria_logger, "Hubo un error al recibir el PC de un proceso");
			}

			t_list* instrucciones = list_get(instrucciones_por_proceso, pid);

			if (instrucciones == NULL) {
				log_warning(memoria_logger, "La lista de instrucciones se encuentra vacia");
			}
			
			char* instruccion = list_get(instrucciones, pc);
			
			usleep(RETARDO_RESPUESTA);
			send_instruccion(fd_cpu, instruccion, strlen(instruccion) + 1);
			log_info(memoria_logger, "Instruccion %d enviada", pc);

			free(instruccion);
			break;
		case -1:
			log_error(memoria_logger, "El CPU se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(memoria_logger, "Operacion desconocida.");
			break;
		}
	}
}