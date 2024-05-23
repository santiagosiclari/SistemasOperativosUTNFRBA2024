#include "../include/instrucciones.h"

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