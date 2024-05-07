#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// Estructura de los registros
typedef struct {
    uint8_t AX, BX, CX, DX;
    uint32_t EAX, EBX, ECX, EDX, SI, DI;
} t_registros;

// Estructura del PCB
typedef struct {
    uint8_t pid;
    uint32_t pc; // Program Counter -> Puntero de instruccion
    char estado; // Es un caracter, es decir N seria New, B Blocked, E Exec, etc.
    uint8_t quantum;
    // falta semaforo
    // falta info de memoria
    // falta info de la programacion (prioridad por ej.)
    // falta info de entrada/salida (e/s)
    t_registros* registros;
} t_pcb;

#endif