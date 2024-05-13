#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <commons/collections/dictionary.h>

#include "../../utils/include/protocolo.h"

// t_pcb* cpu_set(t_pcb* pcb, char** instruccion_separada);
// t_pcb* cpu_sum(t_pcb* pcb, char** instruccion_separada);
// t_pcb* cpu_sub(t_pcb* pcb, char** instruccion_separada);
// t_pcb* cpu_jnz(t_pcb* pcb, char** instruccion_separada);
// t_pcb* cpu_io_gen_sleep(t_pcb* pcb, char** instruccion_separada);

//Registros para determinar el tamanio
// typedef enum{
//     AX,
//     BX,
//     CX,
//     DX,
//     EAX,
//     EBX,
//     ECX,
//     EDX,
//     SI,
//     DI,
//     PC
// } registrosCPU;

// Prueba
void funcion_set(t_dictionary* dictionary_registros, char* registro, int valor);
void funcion_sum(t_dictionary* dictionary_registros, char* registro_destino, char* registro_origen);
void funcion_sub(t_dictionary* dictionary_registros, char* registro_destino, char* registro_origen);
void funcion_jnz(t_dictionary* dictionary_registros, char* registro, uint32_t valor_pc);
void funcion_io_gen_sleep(char* interfaz, uint32_t unidades_trabajo);

extern t_pcb* pcb_a_ejecutar;

// void set(registrosCPU registroDestino, int valor);
// void sum(registrosCPU registroDestino, registrosCPU registroOrigen);
// void sub(registrosCPU registroDestino, registrosCPU registroOrigen);
// void io_gen_sleep(char* interfaz, int unidades_trabajo);
// uint32_t jnz(registrosCPU registro, char* instruccion);

// size_t tamanioRegistro(registrosCPU registro);
// void* obtenerRegistro(registrosCPU registro);

#endif