#ifndef CONFIGS_H_
#define CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/config.h>

t_config* entradasalida_config;

// Declaracion archivos de configuracion
char* TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;
char* IP_KERNEL;
char* PUERTO_KERNEL; //pdf dice int?
char* IP_MEMORIA;
char* PUERTO_MEMORIA;//pdf dice int?
char* PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;

void init_entradasalida_config();

#endif