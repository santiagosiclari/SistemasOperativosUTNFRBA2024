#include "../include/kernel-interfaces.h"

pthread_mutex_t reciboFinDeIO = PTHREAD_MUTEX_INITIALIZER;

void conexion_kernel_interfaces(void* arg) {
    uint8_t MAX_LENGTH = 128;
    int fd_interfaz = *(int*)arg;
    free(arg);
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_interfaz);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
        case FIN_IO:
			pthread_mutex_lock(&reciboFinDeIO);
			char* nombre_fin_io = malloc(MAX_LENGTH);
			char* nombre_fin_io_recibido = malloc(MAX_LENGTH);

			// Busca el socket de la interfaz
			int fd_interfaz = buscar_socket_interfaz(listaInterfaces, nombre_interfaz);

			if(!recv_fin_io(fd_interfaz, nombre_fin_io_recibido)) {
				log_error(kernel_logger, "Hubo un error al recibir el fin de una IO");
			}
			strcpy(nombre_fin_io, nombre_fin_io_recibido);
			
			pthread_mutex_lock(&colaBlockedMutex);
			if(!queue_is_empty(colaBlocked)) {
				t_pcb* pcb_recibido = queue_pop(colaBlocked);
				if(strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0)
				{
					if(pcb_recibido->quantum > 0 && pcb_recibido->quantum < QUANTUM)
					{
						if (pcb_recibido)
						{
							log_info(kernel_logger, "Fin de IO de la interfaz %s del proceso %d", nombre_fin_io, pcb_recibido->pid);
							pthread_mutex_lock(&colaAuxMutex);
							pcb_recibido->estado = 'A';
							pcb_recibido->flag_int = 0;
							queue_push(colaAux, pcb_recibido);
							pthread_mutex_unlock(&colaAuxMutex);
						}
					}
					else{
						if (pcb_recibido) {
							log_info(kernel_logger, "Fin de IO de la interfaz %s del proceso %d", nombre_fin_io, pcb_recibido->pid);
							pthread_mutex_lock(&colaReadyMutex);
							pcb_recibido->estado = 'R';
							pcb_recibido->flag_int = 0;
							queue_push(colaReady, pcb_recibido);
							pthread_mutex_unlock(&colaReadyMutex);
						}
					}
				}
				else{
					if (pcb_recibido) {
						log_info(kernel_logger, "Fin de IO de la interfaz %s del proceso %d", nombre_fin_io, pcb_recibido->pid);
						pthread_mutex_lock(&colaReadyMutex);
						pcb_recibido->estado = 'R';
						pcb_recibido->flag_int = 0;
						queue_push(colaReady, pcb_recibido);
						pthread_mutex_unlock(&colaReadyMutex);
					}
				}
			}
			pthread_mutex_unlock(&colaBlockedMutex);
			pthread_mutex_unlock(&reciboFinDeIO);

			free(nombre_fin_io);
			free(nombre_fin_io_recibido);
			break;
		case -1:
			log_error(kernel_logger, "El cliente de una interfaz IO no se encuentra activo.");
			control = 0;
            break;
		default:
			log_warning(kernel_logger, "Operacion desconocida.");
			break;
		}
	}
}