#ifndef MEMORIA_MAIN_H_
#define MEMORIA_MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "configs.h"
#include "logs.h"

#include "../../utils/include/sockets.h"

#include "memoria-kernel.h"

//variables para conexiones
int fd_memoria;
int fd_kernel;
int fd_entradasalida;
int fd_cpu;

t_config* memoria_config;

// Declaracion archivos de configuracion
char* PUERTO_ESCUCHA; //pdf dice int
int   TAM_MEMORIA;
int   TAM_PAGINA;
char* PATH_INSTRUCCIONES;
int   RETARDO_RESPUESTA;

t_log* memoria_logger;
t_log* memoria_debug;

#endif