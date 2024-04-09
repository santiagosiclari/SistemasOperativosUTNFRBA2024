#ifndef CONFIGS_H_
#define CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/config.h>

t_config* memoria_config;

// Declaracion archivos de configuracion
char* PUERTO_ESCUCHA; //pdf dice int
int TAM_MEMORIA;
int TAM_PAGINA;
char* PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

void init_memoria_config();

#endif