#ifndef ENTRADASALIDA_MAIN_H_
#define ENTRADASALIDA_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <../../utils/include/hello.h>

#include <commons/log.h>
#include <commons/config.h>


//declaracion archivos de configuracion
char* TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;
char* IP_KERNEL;
char* PUERTO_KERNEL; //pdf dice int?
char* IP_MEMORIA;
char* PUERTO_MEMORIA;//pdf dice int?
char* PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;

t_log* entradasalida_logger;
t_log* entradasalida_debug;
t_config* entradasalida_config;

#endif