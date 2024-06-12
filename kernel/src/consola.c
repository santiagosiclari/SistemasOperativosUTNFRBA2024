#include <../include/consola.h>

t_pcb* buscar_pcb_a_finalizar(t_queue* cola, uint8_t pid_a_borrar) {
	// Buscar en cola
	t_pcb* pcb_encontrado = NULL;
    t_queue* cola_temp = queue_create();
    while (!queue_is_empty(cola)) {
        t_pcb* pcb = queue_pop(cola);
        if (pcb->pid == pid_a_borrar) {
            pcb_encontrado = pcb;
			break;
        } else {
            queue_push(cola_temp, pcb);
        }
    }
	// Reconstruir queue
    while (!queue_is_empty(cola_temp)) {
        queue_push(cola, queue_pop(cola_temp));
    }

	queue_destroy(cola_temp);
	return pcb_encontrado;
}

void finalizar_proceso(uint8_t pid_a_borrar) {
	// Liberar memoria
	send_fin_proceso(fd_memoria, pid_a_borrar);
	// Liberar recursos
	liberar_recursos(pid_a_borrar);

	// Si se borra el unico proceso que quedaba entonces detener planificacion
	if (queue_is_empty(colaReady) && queue_is_empty(colaNew) && queue_is_empty(colaExec) && queue_is_empty(colaBlocked) && queue_is_empty(colaAux)) {
		control_planificacion = 0;
	}
}

void buscar_en_queues_y_finalizar(t_pcb* pcb_borrar, uint8_t pid_a_borrar) {
	// New
    pthread_mutex_lock(&colaNewMutex);
	pcb_borrar = buscar_pcb_a_finalizar(colaNew, pid_a_borrar);
    pthread_mutex_unlock(&colaNewMutex);
	if(pcb_borrar != NULL) {
		finalizar_proceso(pid_a_borrar);
		return;
	}

	// Ready
    pthread_mutex_lock(&colaReadyMutex);
	pcb_borrar = buscar_pcb_a_finalizar(colaReady, pid_a_borrar);
    pthread_mutex_unlock(&colaReadyMutex);
	if(pcb_borrar != NULL) {
		finalizar_proceso(pid_a_borrar);
		return;
	}

	// Blocked
    pthread_mutex_lock(&colaBlockedMutex);
	pcb_borrar = buscar_pcb_a_finalizar(colaBlocked, pid_a_borrar);
    pthread_mutex_unlock(&colaBlockedMutex);
	if(pcb_borrar != NULL) {
		finalizar_proceso(pid_a_borrar);
		return;
	}

	// Exec
    pthread_mutex_lock(&colaExecMutex);
	pcb_borrar = buscar_pcb_a_finalizar(colaExec, pid_a_borrar);
    pthread_mutex_unlock(&colaExecMutex);
	if(pcb_borrar != NULL) {
		finalizar_proceso(pid_a_borrar);
		return;
	}
		
	// Aux (Ready prioritaria de VRR)
    pthread_mutex_lock(&colaAuxMutex);
	pcb_borrar = buscar_pcb_a_finalizar(colaAux, pid_a_borrar);
    pthread_mutex_unlock(&colaAuxMutex);
	if(pcb_borrar != NULL) {
		finalizar_proceso(pid_a_borrar);
		return;
	}
		
	// Blocked de cada recurso
	for(int i = 0; i < list_size(recursos); i++) {
		t_recurso* r = list_get(recursos, i);
		pcb_borrar = buscar_pcb_a_finalizar(r->blocked, pid_a_borrar);
		if(pcb_borrar != NULL) {
			finalizar_proceso(pid_a_borrar);
			return;
		}
	}
}

void atender_instruccion (char* leido) {

	char** comando_consola = string_split(leido, " ");
	
	if(strcmp(comando_consola[0], "HELP") == 0) {
		printf( "\nInstrucciones:\n"
				"EJECUTAR_SCRIPT [path]\n"
				"INICIAR_PROCESO [path]\n"
				"FINALIZAR_PROCESO [pid]\n"
				"DETENER_PLANIFICACION\n"
				"INICIAR_PLANIFICACION\n"
				"MULTIPROGRAMACION [valor]\n"
				"PROCESO_ESTADO\n\n");
	} else if(strcmp(comando_consola[0], "INICIAR_PROCESO") == 0) {
		t_pcb* pcb = crear_pcb();
		send_pcb(fd_cpu_dispatch, pcb);
		log_info(kernel_logger, "Se crea el proceso %d en NEW", pcb->pid);

        pthread_mutex_lock(&colaNewMutex);
		queue_push(colaNew, pcb);
        pthread_mutex_unlock(&colaNewMutex);

		t_list* recursos_tomados = list_create();
		list_add(recursos_de_procesos, recursos_tomados);

		send_iniciar_proceso(fd_memoria, pcb->pid, comando_consola[1], strlen(comando_consola[1]) + 1);
		log_info(kernel_logger, "Path enviado: %s", comando_consola[1]);
	} else if (strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0) {
    	pthread_t planificacion;
		if(strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0) {
			pthread_create(&planificacion, NULL, (void *)planificacionFIFO, NULL);
		} else if(strcmp(ALGORITMO_PLANIFICACION, "RR") == 0) {
			pthread_create(&planificacion, NULL, (void *)planificacionRR, NULL);
		} else if(strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
			pthread_create(&planificacion, NULL, (void *)planificacionVRR, NULL);
		}
		pthread_detach(planificacion); 
	} else if (strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0) {
		// Buscar pid en queue --> pid => comando_consola[1]
		uint8_t pid_a_borrar = atoi(comando_consola[1]);
		t_pcb* pcb_borrar = malloc(sizeof(t_pcb));
		pcb_borrar->registros = malloc(sizeof(t_registros));

		buscar_en_queues_y_finalizar(pcb_borrar, pid_a_borrar);

		free(pcb_borrar->registros);
		free(pcb_borrar);
	} else {
		log_warning(kernel_logger, "ERROR. No se encontro el comando. Escribi HELP si necesitas ayuda con los comandos y sus parametros");
	}

	// Liberar cada elemento
	int i = 0;
    while (comando_consola[i] != NULL) {
        free(comando_consola[i]);
        i++;
    }

    // Liberar el array
    free(comando_consola);
}

void iniciar_consola(t_log* logger) {
    char* leido;
	leido = readline("> ");

	while (strcmp(leido,"") != 0) {
		atender_instruccion(leido);
        free(leido);
		leido = readline("> ");
	}
	
	free(leido);
}