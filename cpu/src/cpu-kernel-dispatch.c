#include "../include/cpu-kernel-dispatch.h"

t_pcb* pcb_a_ejecutar;
t_list* procesos;

void conexion_cpu_kernel_dispatch() {
    bool control = 1;
	procesos = list_create();
	while (control) {
		int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op) {
		case RECIBIR_PCB:
			// Recibo PCB
			t_pcb* pcb = malloc(sizeof(t_pcb));
			pcb->registros = malloc(sizeof(t_registros));
			
			if(!recv_pcb(fd_kernel_dispatch, pcb)) {
				log_error(cpu_logger, "Hubo un error al recibir el PCB del modulo Kernel (Dispatch)");
				free(pcb->registros);
				free(pcb);
				exit(EXIT_FAILURE);
			}

			// Controlo si es nulo
			if (pcb == NULL) {
				log_error(cpu_logger, "El PCB recibido es NULL");
				exit(EXIT_FAILURE);
			}

			list_add_in_index(procesos, pcb->pid, pcb);

			// Printea el PCB
			log_info(cpu_logger, "PID: %d\nProgram Counter: %d\nEstado: %c\nQuantum: %d\nFlag IO: %d\nRegistros:\nAX: %d, BX: %d, CX: %d, DX: %d\nEAX: %d, EBX: %d, ECX: %d, EDX: %d\nSI: %d, DI: %d",
			pcb->pid, pcb->pc, pcb->estado, pcb->quantum, pcb->flag_io,
			pcb->registros->AX, pcb->registros->BX, pcb->registros->CX, pcb->registros->DX,
			pcb->registros->EAX, pcb->registros->EBX, pcb->registros->ECX, pcb->registros->EDX,
			pcb->registros->SI, pcb->registros->DI);

            break;
		case RECIBIR_PID:
			// Recibo proceso a ejecutar
			uint8_t pid_a_ejecutar;
			if(!recv_pid(fd_kernel_dispatch, &pid_a_ejecutar)) {
				log_error(cpu_logger, "Hubo un error al recibir el PID.");
			} else {
				log_info(cpu_logger, "PID a ejecutar: %d", pid_a_ejecutar);
			}

			pcb_a_ejecutar = list_get(procesos, pid_a_ejecutar);

			// Fetch --> pedir la primera instruccion y despues pasa al while en cpu-memoria.c
			send_pc_pid(fd_memoria, pcb_a_ejecutar->pc, pcb_a_ejecutar->pid);
			log_info(cpu_logger, "Se envio el PC %d a memoria", pcb_a_ejecutar->pc);
			break;
		case -1:
			log_error(cpu_logger, "El Kernel (Dispatch) se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(cpu_logger, "Operacion desconocida.");
			break;
		}
	}
    list_destroy_and_destroy_elements(procesos, free);
}