#ifndef KERNELL_MAIN_H_
#define KERNELL_MAIN_H_

#include<commons/log.h>
#include<commons/config.h>

//declaracion archivos de configuracion
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

t_log* kernel_logger;
t_log* kernel_debug;
t_config* kernel_config;


#endif