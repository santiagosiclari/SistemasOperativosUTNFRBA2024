#include "../include/instrucciones.h"

// int mmu(int dir_logica) {
//     // Realizo calculos
// 	// int numeroPagina = floor(dir_logica / tam_pagina);
// 	// int desplazamiento = dir_logica - numeroPagina * tam_pagina;

// 	// t_tlb* tlb_encontrada = buscar_en_tlb(numeroPagina);
// 	// if (tlb_encontrada != NULL){
//     //     log_info(cpu_logger, "PID: %d- TLB HIT - Pagina: %d", pcb_a_ejecutar->pid, numeroPagina);
// 	// 	   return (tlb_encontrada->numMarco) + desplazamiento;
// 	// } else {
// 	// 	log_info(cpu_logger, "PID: %d- TLB MISS - Pagina: %d", pcb_a_ejecutar->pid, numeroPagina);
// 	// 	send --> mandar_numero_actualizado(conexion_memoria, numeroPagina, BUSCAR_MARCO);

//     //     Esto lo vamos a hacer en los hilos de cpu-memoria
// 	// 	op_code codOp = recibir_operacion(conexion_memoria);
// 	// 	if(codOp == NUMERO){
//     //  		log_debug(cpu_logger,"Entre a numero bien");
// 	// 		int marco = 0;
// 	// 		marco = recibir_numero(conexion_memoria);
// 	// 		if(list_size(listaTLB)<config.cantidad_entradas_tlb){
// 	// 			agregar_a_tlb(pcb_a_ejecutar->pid,numeroPagina,marco);
// 	// 		}else{
// 	// 			//el algoritmoSustitucion debe estar en un hilo?
// 	// 			algoritmoSustitucion(pcb_a_ejecutar->pid,numeroPagina,marco);
// 	// 		}
// 	// 	  	return marco+desplazamiento;
//     // 	}
// 	// }

// 	return -1;
// }

int mmu(int dir_logica) {
    // Realizo calculos
    uint32_t numero_pagina = dir_logica / tam_pagina;
    uint32_t desplazamiento = dir_logica - numero_pagina * tam_pagina;

    t_tlb* entrada_tlb = buscar_en_tlb(pcb_a_ejecutar->pid, numero_pagina);
    if (entrada_tlb != NULL) {
        // TLB Hit
        log_info(cpu_logger, "PID: %d- TLB HIT - Pagina: %d", pcb_a_ejecutar->pid, numero_pagina);
        return (entrada_tlb->marco * tam_pagina) + desplazamiento;
    } else {
        // TLB Miss
        log_info(cpu_logger, "PID: %d- TLB HIT - Pagina: %d", pcb_a_ejecutar->pid, numero_pagina);
        // send_num_pagina(fd_memoria, entrada_tlb->pid, numero_pagina);
        return -1; // Significa que es un TLB Miss y esta esperando recibir el marco

        // Esto se hace cuando recibe el codop RECIBIR_MARCO
        // int marco = solicitar_marco_a_memoria(pid, numero_pagina);
        // agregar_a_tlb(pid, numero_pagina, marco);
        // return (marco * tam_pagina) + desplazamiento;
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
    send_tamanio(fd_memoria, tamanio);
    log_info(cpu_logger, "Funcion Resize enviada a memoria: %d", tamanio);
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