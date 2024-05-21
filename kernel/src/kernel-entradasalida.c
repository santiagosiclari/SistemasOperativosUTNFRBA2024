#include "../include/kernel-entradasalida.h"

void conexion_kernel_entradasalida() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_entradasalida);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case FIN_IO:
			t_pcb* pcb_fin_io = malloc(sizeof(t_pcb));
			pcb_fin_io->registros = malloc(sizeof(t_registros));
			uint32_t MAX_LENGTH = 128;
			char* nombre = malloc(MAX_LENGTH);
			char* nombre_recivido = malloc(MAX_LENGTH);

			if(!recv_fin_io(fd_entradasalida, pcb_fin_io, nombre_recivido)) {
				log_error(kernel_logger, "Hubo un error al recibir el fin de una IO");
			}
			strcpy(nombre, nombre_recivido);
			
			pthread_mutex_lock(&colaBlockedMutex);
			queue_pop(colaBlocked);
			log_info(kernel_logger, "Fin de IO de la interfaz %s del proceso %d", nombre, pcb_fin_io->pid);
			// Printea el PCB
				log_info(kernel_logger, "PID: %d\nProgram Counter: %d\nEstado: %c\nQuantum: %d\nFlag IO: %d\nRegistros:\nAX: %d, BX: %d, CX: %d, DX: %d\nEAX: %d, EBX: %d, ECX: %d, EDX: %d\nSI: %d, DI: %d",
				pcb_fin_io->pid, pcb_fin_io->pc, pcb_fin_io->estado, pcb_fin_io->quantum, pcb_fin_io->flag_int,
				pcb_fin_io->registros->AX, pcb_fin_io->registros->BX, pcb_fin_io->registros->CX, pcb_fin_io->registros->DX,
				pcb_fin_io->registros->EAX, pcb_fin_io->registros->EBX, pcb_fin_io->registros->ECX, pcb_fin_io->registros->EDX,
				pcb_fin_io->registros->SI, pcb_fin_io->registros->DI);
			pthread_mutex_unlock(&colaBlockedMutex);

			pthread_mutex_lock(&colaReadyMutex);
			pcb_fin_io->estado = 'R';
			pcb_fin_io->flag_int = 0;
			// Printea el PCB
				log_info(kernel_logger, "PID: %d\nProgram Counter: %d\nEstado: %c\nQuantum: %d\nFlag IO: %d\nRegistros:\nAX: %d, BX: %d, CX: %d, DX: %d\nEAX: %d, EBX: %d, ECX: %d, EDX: %d\nSI: %d, DI: %d",
				pcb_fin_io->pid, pcb_fin_io->pc, pcb_fin_io->estado, pcb_fin_io->quantum, pcb_fin_io->flag_int,
				pcb_fin_io->registros->AX, pcb_fin_io->registros->BX, pcb_fin_io->registros->CX, pcb_fin_io->registros->DX,
				pcb_fin_io->registros->EAX, pcb_fin_io->registros->EBX, pcb_fin_io->registros->ECX, pcb_fin_io->registros->EDX,
				pcb_fin_io->registros->SI, pcb_fin_io->registros->DI);

			queue_push(colaReady, pcb_fin_io);

			// Printea el PCB
				log_info(kernel_logger, "PID: %d\nProgram Counter: %d\nEstado: %c\nQuantum: %d\nFlag IO: %d\nRegistros:\nAX: %d, BX: %d, CX: %d, DX: %d\nEAX: %d, EBX: %d, ECX: %d, EDX: %d\nSI: %d, DI: %d",
				pcb_fin_io->pid, pcb_fin_io->pc, pcb_fin_io->estado, pcb_fin_io->quantum, pcb_fin_io->flag_int,
				pcb_fin_io->registros->AX, pcb_fin_io->registros->BX, pcb_fin_io->registros->CX, pcb_fin_io->registros->DX,
				pcb_fin_io->registros->EAX, pcb_fin_io->registros->EBX, pcb_fin_io->registros->ECX, pcb_fin_io->registros->EDX,
				pcb_fin_io->registros->SI, pcb_fin_io->registros->DI);

			pthread_mutex_unlock(&colaReadyMutex);

			free(pcb_fin_io->registros);
			free(pcb_fin_io);
			free(nombre);
			free(nombre_recivido);
			break;
		case -1:
			log_error(kernel_logger, "El IO se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(kernel_logger, "Operacion desconocida.");
			break;
		}
	}
}