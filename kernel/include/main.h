#ifndef KERNEL_MAIN_H_
#define KERNEL_MAIN_H_

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

t_config* kernel_config;

t_log* kernel_logger;
t_log* kernel_debug;

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

#endif