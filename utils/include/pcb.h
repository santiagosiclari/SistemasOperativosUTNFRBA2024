#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// Estructura de los registros
typedef struct {
    uint8_t AX, BX, CX, DX;
    uint32_t EAX, EBX, ECX, EDX, SI, DI, PC;
} t_registros;

extern t_registros miCPU;

// Estructura del PCB
typedef struct {
    uint8_t pid;
    uint32_t pc; // Program Counter -> Puntero de instruccion
    char estado; // Es un caracter, es decir N seria New, B Blocked, E Exec, etc.
    uint32_t quantum;
    // uint8_t flag_io; // Cuando Kernel recibe el Contexto de Ejecucion, revisa porque fue interrumpido --> si flag_io == 1 es por una interrupcion de IO
    t_registros* registros;
} t_pcb;

#endif