#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <commons/collections/dictionary.h>

#include "../include/tlb.h"
#include "../../utils/include/protocolo.h"

extern uint32_t tam_pagina;

// MMU
int mmu(int dir_logica);

// Instrucciones
void funcion_set(t_dictionary* dictionary_registros, char* registro, int valor);
void funcion_sum(t_dictionary* dictionary_registros, char* registro_destino, char* registro_origen);
void funcion_sub(t_dictionary* dictionary_registros, char* registro_destino, char* registro_origen);
void funcion_jnz(t_dictionary* dictionary_registros, char* registro, uint32_t valor_pc);
void funcion_resize(uint32_t tamanio);
void funcion_io_gen_sleep(char* interfaz, uint32_t unidades_trabajo);
void funcion_exit();

extern t_pcb* pcb_a_ejecutar;
extern t_log* cpu_logger;

#endif