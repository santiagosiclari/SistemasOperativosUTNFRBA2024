#ifndef MEMORIA_MAIN_H_
#define MEMORIA_MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <../../utils/include/hello.h>

#include <commons/log.h>
#include <commons/config.h>

char* PUERTO_ESCUCHA; //pdf dice int
int TAM_MEMORIA;
int TAM_PAGINA;
char* PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

t_log* memoria_logger;
t_log* memoria_debug;
t_config* memoria_config;


#endif