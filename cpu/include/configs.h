#ifndef CONFIGS_H_
#define CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/config.h>

t_config* cpu_config;

// Declaracion archivos de configuracion
char* IP_MEMORIA;
char* PUERTO_MEMORIA; //char para func
char* PUERTO_ESCUCHA_DISPATCH;//char para func
char* PUERTO_ESCUCHA_INTERRUPT;//char para func
int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

void init_cpu_config();

#endif