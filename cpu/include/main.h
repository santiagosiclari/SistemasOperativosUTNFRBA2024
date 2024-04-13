#ifndef CPU_MAIN_H_
#define CPU_MAIN_H_

#include <stdlib.h>
#include <stdio.h>

#include "configs.h"
#include "logs.h"

#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"

int fd_kernel;
int fd_memoria;
int fd_entradasalida;
int fd_cpu_dispatch;
int fd_cpu_interrupt;

t_log* cpu_logger;
t_log* cpu_debug;

t_config* cpu_config;

// Declaracion archivos de configuracion
char* IP_MEMORIA;
char* PUERTO_MEMORIA; //char para func
char* PUERTO_ESCUCHA_DISPATCH;//char para func
char* PUERTO_ESCUCHA_INTERRUPT;//char para func
int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

#endif