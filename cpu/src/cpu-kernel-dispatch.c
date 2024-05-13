#include "../include/cpu-kernel-dispatch.h"

t_pcb* pcb_a_ejecutar;

void conexion_cpu_kernel_dispatch() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op) {
		case RECIBIR_PCB:
			// Recibo PCB
			t_pcb* pcb = malloc(sizeof(t_pcb));
			pcb->registros = malloc(sizeof(t_registros));
			t_queue* procesos = queue_create();
			
			if(!recv_pcb(fd_kernel_dispatch, pcb)) {
				log_error(cpu_logger, "Hubo un error al recibir el PCB del modulo Kernel (Dispatch)");
				exit(EXIT_FAILURE);
			}

			// Controlo si es nulo
			if (pcb == NULL) {
				log_error(cpu_logger, "El PCB recibido es NULL");
				exit(EXIT_FAILURE);
			}

			queue_push(procesos, pcb);

			// Printea el PCB
			printf("PID: %d\n", pcb->pid);
			printf("Program Counter: %d\n", pcb->pc);
			printf("Estado: %c\n", pcb->estado);
			printf("Quantum: %d\n", pcb->quantum);
			printf("Registros:\n");
			printf("AX: %d, BX: %d, CX: %d, DX: %d\n", pcb->registros->AX, pcb->registros->BX, pcb->registros->CX, pcb->registros->DX);
			printf("EAX: %d, EBX: %d, ECX: %d, EDX: %d\n", pcb->registros->EAX, pcb->registros->EBX, pcb->registros->ECX, pcb->registros->EDX);
			printf("SI: %d, DI: %d\n", pcb->registros->SI, pcb->registros->DI);

            break;
		case RECIBIR_PID:
			// Recibo proceso a ejecutar
			uint8_t pid_a_ejecutar;
			if(!recv_pid(fd_kernel_dispatch, pid_a_ejecutar)) {
				log_error(cpu_logger, "Hubo un error al recibir el PID.");
			} else {
				log_info(cpu_logger, "PID a ejecutar: %d", pid_a_ejecutar);
			}

			t_queue* procesos_a_ejecutar = queue_create();
			queue_push(procesos_a_ejecutar, &pid_a_ejecutar);

			// Wait semaforo
			while(queue_size(procesos_a_ejecutar) > 0) {
				// Agarro el pid a ejecutar con un pid temporal
				uint8_t pid_temp;
				pid_temp = *(uint8_t*)queue_peek(procesos_a_ejecutar);
				pid_a_ejecutar = pid_temp;
				pcb_a_ejecutar = queue_peek(procesos);
				if(pcb_a_ejecutar->pid == pid_a_ejecutar) {
					while(pcb_a_ejecutar->pc <= size_instrucciones) {
						// Fetch --> pedir instruccion
						send_pc(fd_memoria, pcb_a_ejecutar->pc);
						log_info(cpu_logger, "Se envio el PC %d a memoria", pcb_a_ejecutar->pc);
					}
				}
			}
			// Signal semaforo
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
}