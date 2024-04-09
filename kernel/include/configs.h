#ifndef CONFIGS_H_
#define CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/config.h>

t_config* kernel_config;

// Declaracion archivos de configuracion
char* PUERTO_ESCUCHA; //pdf dice int
char* IP_MEMORIA;
char* PUERTO_MEMORIA;//pdf dice int
char* IP_CPU;
char* PUERTO_CPU_DISPATCH;//pdf dice int
char* PUERTO_CPU_INTERRUPT;//pdf dice int
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char** RECURSOS;//lista, luego usar common de string
char** INSTANCIAS_RECURSOS;//lista, luego usar common de string
int GRADO_MULTIPROGRAMACION;

void init_kernel_config();

#endif