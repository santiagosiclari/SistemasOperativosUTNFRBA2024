#ifndef CPU_MAIN_H_
#define CPU_MAIN_H_

#include<commons/log.h>
#include<commons/config.h>


//declaracion archivos de configuracion

char* IP_MEMORIA;
char* PUERTO_MEMORIA; //char para func
char* PUERTO_ESCUCHA_DISPATCH;//char para func
char* PUERTO_ESCUCHA_INTERRUPT;//char para func
int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

t_log* cpu_logger;
t_log* cpu_debug;
t_config* cpu_config;

#endif