#include "../include/instrucciones.h"

t_instruccion_pendiente* instruccion_pendiente;
pthread_mutex_t instruccion_pendiente_mutex = PTHREAD_MUTEX_INITIALIZER;

uint32_t mmu(uint32_t dir_logica) {
    // Realizo calculos
    uint32_t numero_pagina = dir_logica / tam_pagina;
    uint32_t desplazamiento = dir_logica - numero_pagina * tam_pagina;

    log_info(cpu_logger, "Dir logica: %u, Tam pagina: %u, Numero de pagina: %u, Desplazamiento: %u", dir_logica, tam_pagina, numero_pagina, desplazamiento);

    t_tlb* entrada_tlb = buscar_en_tlb(pcb_a_ejecutar->pid, numero_pagina);
    if (entrada_tlb != NULL) {
        // TLB Hit
        log_info(cpu_logger, "PID: %d - TLB HIT - Pagina: %d", pcb_a_ejecutar->pid, numero_pagina);
        // Mover marco para LRU
        if (strcmp(ALGORITMO_TLB, "LRU") == 0) {
            // El marco mas recientemente usado va a ir al final de la lista (index: list_size(lista_tlb) - 1)
            // mientras que el menos recientemente usado va al principio (index: 0)
            int index = buscar_indice_en_lista(lista_tlb, entrada_tlb);
            if (index != -1) {
                mover_elemento_al_principio(lista_tlb, index);
            }
        }
        return (entrada_tlb->marco * tam_pagina) + desplazamiento;
    } else {
        // TLB Miss
        log_info(cpu_logger, "PID: %d - TLB MISS - Pagina: %d", pcb_a_ejecutar->pid, numero_pagina);
        send_num_pagina(fd_memoria, pcb_a_ejecutar->pid, numero_pagina, desplazamiento);
        return -1; // Significa que es un TLB Miss y esta esperando recibir el marco
    }
}

void funcion_set(t_dictionary* dictionary_registros, char* registro, int valor) {
    if (strlen(registro) == 3 || !strcmp(registro, "SI") || !strcmp(registro, "DI") || !strcmp(registro, "PC")) {
        uint32_t *r_destino = dictionary_get(dictionary_registros, registro);
        *r_destino = valor;
    } else if (strlen(registro) == 2) {
        uint8_t *r_destino = dictionary_get(dictionary_registros, registro);
        *r_destino = valor;
    }

    pcb_a_ejecutar->pc++;
}

void funcion_sum(t_dictionary* dictionary_registros, char* registro_destino, char* registro_origen) {
    if (strlen(registro_destino) == 3 || !strcmp(registro_destino, "SI") || !strcmp(registro_destino, "DI") || !strcmp(registro_destino, "PC")) {
        uint32_t *r_destino = dictionary_get(dictionary_registros, registro_destino);
        if (strlen(registro_origen) == 3 || !strcmp(registro_origen, "SI") || !strcmp(registro_origen, "DI") || !strcmp(registro_origen, "PC")) {
            uint32_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino += *r_origen;
        } else if (strlen(registro_origen) == 2) {
            uint8_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino += *r_origen;
        }
    } else if (strlen(registro_destino) == 2) {
        uint8_t *r_destino = dictionary_get(dictionary_registros, registro_destino);
        if (strlen(registro_origen) == 3 || !strcmp(registro_origen, "SI") || !strcmp(registro_origen, "DI") || !strcmp(registro_origen, "PC")) {
            uint32_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino += *r_origen;
        } else if (strlen(registro_origen) == 2) {
            uint8_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino += *r_origen;
        }
    }

    pcb_a_ejecutar->pc++;
}

void funcion_sub(t_dictionary* dictionary_registros, char* registro_destino, char* registro_origen) {
    if (strlen(registro_destino) == 3 || !strcmp(registro_destino, "SI") || !strcmp(registro_destino, "DI") || !strcmp(registro_destino, "PC")) {
        uint32_t *r_destino = dictionary_get(dictionary_registros, registro_destino);
        if (strlen(registro_origen) == 3 || !strcmp(registro_origen, "SI") || !strcmp(registro_origen, "DI") || !strcmp(registro_origen, "PC")) {
            uint32_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino -= *r_origen;
        } else if (strlen(registro_origen) == 2) {
            uint8_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino -= *r_origen;
        }
    } else if (strlen(registro_destino) == 2) {
        uint8_t *r_destino = dictionary_get(dictionary_registros, registro_destino);
        if (strlen(registro_origen) == 3 || !strcmp(registro_origen, "SI") || !strcmp(registro_origen, "DI") || !strcmp(registro_origen, "PC")) {
            uint32_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino -= *r_origen;
        } else if (strlen(registro_origen) == 2) {
            uint8_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino -= *r_origen;
        }
    }

    pcb_a_ejecutar->pc++;
}

void funcion_mov_in(t_dictionary* dictionary_registros, char* registro_datos, char* registro_direccion) {
    // Obtener la direccion logica
    int direccion_fisica;
    if (strlen(registro_direccion) == 3 || !strcmp(registro_direccion, "SI") || !strcmp(registro_direccion, "DI") || !strcmp(registro_direccion, "PC")) {
        uint32_t *dir_logica = dictionary_get(dictionary_registros, registro_direccion);
        direccion_fisica = mmu(*dir_logica);
        if (direccion_fisica == -1) {
            // TLB miss, guardar la instruccion pendiente y esperar a recibir el marco
            pthread_mutex_lock(&instruccion_pendiente_mutex);
            instruccion_pendiente = malloc(sizeof(t_instruccion_pendiente));
            instruccion_pendiente->instruccion = "MOV_IN";
            instruccion_pendiente->registro_datos = strdup(registro_datos);
            instruccion_pendiente->registro_direccion = strdup(registro_direccion);
            instruccion_pendiente->direccion_logica = *dir_logica;
            pthread_mutex_unlock(&instruccion_pendiente_mutex);
            return;
        }
	} else if (strlen(registro_direccion) == 2) {
        uint8_t *dir_logica = dictionary_get(dictionary_registros, registro_direccion);
        direccion_fisica = mmu(*dir_logica);
        if (direccion_fisica == -1) {
            // TLB miss, guardar la instruccion pendiente y esperar a recibir el marco
            pthread_mutex_lock(&instruccion_pendiente_mutex);
            instruccion_pendiente = malloc(sizeof(t_instruccion_pendiente));
            instruccion_pendiente->instruccion = "MOV_IN";
            instruccion_pendiente->registro_datos = strdup(registro_datos);
            instruccion_pendiente->registro_direccion = strdup(registro_direccion);
            instruccion_pendiente->direccion_logica = *dir_logica;
            pthread_mutex_unlock(&instruccion_pendiente_mutex);
            return;
        }
    }

    // TLB hit, continuar con la ejecucion normal de mov_in
    if (strlen(registro_datos) == 3 || !strcmp(registro_datos, "SI") || !strcmp(registro_datos, "DI") || !strcmp(registro_datos, "PC")) {
        uint32_t tamanio_a_leer = sizeof(uint32_t);
		send_leer_memoria(fd_memoria, pcb_a_ejecutar->pid, direccion_fisica, tamanio_a_leer);
        pthread_mutex_lock(&instruccion_pendiente_mutex);
        instruccion_pendiente = malloc(sizeof(t_instruccion_pendiente));
        instruccion_pendiente->instruccion = "MOV_IN";
        instruccion_pendiente->registro_datos = strdup(registro_datos);
        instruccion_pendiente->registro_direccion = strdup(registro_direccion);
        pthread_mutex_unlock(&instruccion_pendiente_mutex);
	} else if (strlen(registro_datos) == 2) {
        uint32_t tamanio_a_leer = sizeof(uint8_t);
		send_leer_memoria(fd_memoria, pcb_a_ejecutar->pid, direccion_fisica, tamanio_a_leer);
        pthread_mutex_lock(&instruccion_pendiente_mutex);
        instruccion_pendiente = malloc(sizeof(t_instruccion_pendiente));
        instruccion_pendiente->instruccion = "MOV_IN";
        instruccion_pendiente->registro_datos = strdup(registro_datos);
        instruccion_pendiente->registro_direccion = strdup(registro_direccion);
        pthread_mutex_unlock(&instruccion_pendiente_mutex);
	}

    pcb_a_ejecutar->pc++;
}

void funcion_mov_out(t_dictionary* dictionary_registros, char* registro_direccion, char* registro_datos) {
    // Obtener la direccion logica
    int direccion_fisica;
    if (strlen(registro_direccion) == 3 || !strcmp(registro_direccion, "SI") || !strcmp(registro_direccion, "DI") || !strcmp(registro_direccion, "PC")) {
        uint32_t *dir_logica = dictionary_get(dictionary_registros, registro_direccion);
        direccion_fisica = mmu(*dir_logica);
        if (direccion_fisica == -1) {
            // TLB miss, guardar la instruccion pendiente y esperar a recibir el marco
            pthread_mutex_lock(&instruccion_pendiente_mutex);
            instruccion_pendiente = malloc(sizeof(t_instruccion_pendiente));
            instruccion_pendiente->instruccion = "MOV_OUT";
            instruccion_pendiente->registro_datos = strdup(registro_datos);
            instruccion_pendiente->registro_direccion = strdup(registro_direccion);
            instruccion_pendiente->direccion_logica = *dir_logica;
            pthread_mutex_unlock(&instruccion_pendiente_mutex);
            return;
        }
	} else if (strlen(registro_direccion) == 2) {
        uint8_t *dir_logica = dictionary_get(dictionary_registros, registro_direccion);
        direccion_fisica = mmu(*dir_logica);
        if (direccion_fisica == -1) {
            // TLB miss, guardar la instruccion pendiente y esperar a recibir el marco
            pthread_mutex_lock(&instruccion_pendiente_mutex);
            instruccion_pendiente = malloc(sizeof(t_instruccion_pendiente));
            instruccion_pendiente->instruccion = "MOV_OUT";
            instruccion_pendiente->registro_datos = strdup(registro_datos);
            instruccion_pendiente->registro_direccion = strdup(registro_direccion);
            instruccion_pendiente->direccion_logica = *dir_logica;
            pthread_mutex_unlock(&instruccion_pendiente_mutex);
            return;
        }
    }
    
    // TLB hit, continuar con la ejecucion normal de mov_out
    if (strlen(registro_datos) == 3 || !strcmp(registro_datos, "SI") || !strcmp(registro_datos, "DI") || !strcmp(registro_datos, "PC")) {
		uint32_t *reg_datos = dictionary_get(dictionary_registros, registro_datos);
		uint32_t tamanio_a_escribir = sizeof(uint32_t);
		send_escribir_memoria(fd_memoria, pcb_a_ejecutar->pid, direccion_fisica, reg_datos, tamanio_a_escribir);
		log_info(cpu_logger, "MOV_OUT: PID: %d, Direccion fisica: %d, Valor: %d, Tamaño: %d", pcb_a_ejecutar->pid, direccion_fisica, *reg_datos, tamanio_a_escribir);
	} else if (strlen(registro_datos) == 2) {
		uint8_t *reg_datos = dictionary_get(dictionary_registros, registro_datos);
		uint32_t tamanio_a_escribir = sizeof(uint8_t);
	    send_escribir_memoria(fd_memoria, pcb_a_ejecutar->pid, direccion_fisica, reg_datos, tamanio_a_escribir);
		log_info(cpu_logger, "MOV_OUT: PID: %d, Direccion fisica: %d, Valor: %d, Tamaño: %d", pcb_a_ejecutar->pid, direccion_fisica, *reg_datos, tamanio_a_escribir);
	}

    pcb_a_ejecutar->pc++;
}

void funcion_jnz(t_dictionary* dictionary_registros, char* registro, uint32_t valor_pc) {
    if (strlen(registro) == 3 || !strcmp(registro, "SI") || !strcmp(registro, "DI") || !strcmp(registro, "PC")) {
        uint32_t *r_registro = dictionary_get(dictionary_registros, registro);
        if (*r_registro != 0) {
            pcb_a_ejecutar->pc = valor_pc;
        } else {
            pcb_a_ejecutar->pc++;
        }
    } else if (strlen(registro) == 2) {
        uint8_t *r_registro = dictionary_get(dictionary_registros, registro);
        if(*r_registro != 0) { 
            pcb_a_ejecutar->pc = valor_pc;
        } else {
            pcb_a_ejecutar->pc++;
        }
    }
}

void funcion_resize(uint32_t tamanio) {
    send_tamanio(fd_memoria, tamanio, pcb_a_ejecutar->pid);
    log_info(cpu_logger, "Funcion Resize enviada a memoria: %d", tamanio);
    pcb_a_ejecutar->pc++;
}

void funcion_io_gen_sleep(char* interfaz, uint32_t unidades_trabajo) {
    pcb_a_ejecutar->flag_int = 1; // Cuando envio el Contexto de ejecucion al Kernel, sabe que el proceso fue interrumpido por una interfaz IO
    pcb_a_ejecutar->pc++;
    // Enviar el pcb como contexto de ejecucion directamente en la funcion de send_io_gen_sleep
    send_io_gen_sleep(fd_kernel_dispatch, pcb_a_ejecutar, unidades_trabajo, interfaz, strlen(interfaz) + 1); // Envia pcb y el nombre de la interfaz
}

void funcion_exit() {
    send_pid_a_borrar(fd_kernel_dispatch, pcb_a_ejecutar->pid);
    free(pcb_a_ejecutar->registros);
    free(pcb_a_ejecutar);
    pcb_a_ejecutar = NULL;
}