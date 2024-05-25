#include "../include/memoria-interfaces.h"

void conexion_memoria_interfaces(void* arg) {
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
		case -1:
			log_error(memoria_logger, "El cliente de una interfaz IO no se encuentra activo.");
			control = 0;
            break;
		default:
			log_warning(memoria_logger, "Operacion desconocida.");
			break;
		}
	}
}