#include "../include/kernel-entradasalida.h"

pthread_mutex_t reciboFinDeIO = PTHREAD_MUTEX_INITIALIZER;

void conexion_kernel_entradasalida() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_entradasalida);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case RECIBIR_NOMBRE_IO:
			t_interfaz* interfaz = malloc(sizeof(t_interfaz));
			uint8_t MAX_LENGTH = 128;
			char* nombre = malloc(MAX_LENGTH);
			char* nombre_recibido = malloc(MAX_LENGTH);
			
			// Recibo nombre de la interfaz conectada
			if(!recv_interfaz(fd_entradasalida, nombre_recibido)) {
				log_error(kernel_logger, "Hubo un erro al recibir el nombre de la interfaz.");
			}
			strcpy(nombre, nombre_recibido);

			// Guarda en una lista por cada interfaz (t_interfaz)
			interfaz->nombre = nombre;
			interfaz->socket = fd_entradasalida;
			list_add(listaInterfaces, interfaz);

			// Liberar memoria
			break;
		case FIN_IO:
			pthread_mutex_lock(&reciboFinDeIO);
			t_pcb* pcb_fin_io = malloc(sizeof(t_pcb));
			pcb_fin_io->registros = malloc(sizeof(t_registros));
			char* nombre_fin_io = malloc(MAX_LENGTH);
			char* nombre_fin_io_recibido = malloc(MAX_LENGTH);

			// Busca el socket de la interfaz
			uint8_t fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_interfaz);
			
			if(!recv_fin_io(fd_interfaz, pcb_fin_io, nombre_fin_io_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir el fin de una IO");
			}
			strcpy(nombre_fin_io, nombre_fin_io_recibido);
			
			pthread_mutex_lock(&colaBlockedMutex);
			log_info(kernel_logger, "Fin de IO de la interfaz %s del proceso %d", nombre_fin_io, pcb_fin_io->pid);
			if(!queue_is_empty(colaBlocked)) {
				queue_pop(colaBlocked);
			}
			// // Printea el PCB
			// 	log_info(kernel_logger, "PID: %d\nProgram Counter: %d\nEstado: %c\nQuantum: %d\nFlag IO: %d\nRegistros:\nAX: %d, BX: %d, CX: %d, DX: %d\nEAX: %d, EBX: %d, ECX: %d, EDX: %d\nSI: %d, DI: %d",
			// 	pcb_fin_io->pid, pcb_fin_io->pc, pcb_fin_io->estado, pcb_fin_io->quantum, pcb_fin_io->flag_int,
			// 	pcb_fin_io->registros->AX, pcb_fin_io->registros->BX, pcb_fin_io->registros->CX, pcb_fin_io->registros->DX,
			// 	pcb_fin_io->registros->EAX, pcb_fin_io->registros->EBX, pcb_fin_io->registros->ECX, pcb_fin_io->registros->EDX,
			// 	pcb_fin_io->registros->SI, pcb_fin_io->registros->DI);
			pthread_mutex_unlock(&colaBlockedMutex);

			pthread_mutex_lock(&colaReadyMutex);
			pcb_fin_io->estado = 'R';
			pcb_fin_io->flag_int = 0;
			queue_push(colaReady, pcb_fin_io);
			pthread_mutex_unlock(&colaReadyMutex);
			pthread_mutex_unlock(&reciboFinDeIO);

			// free(pcb_fin_io->registros);
			// free(pcb_fin_io);
			free(nombre_fin_io);
			free(nombre_fin_io_recibido);
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