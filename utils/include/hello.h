#ifndef UTILS_HELLO_H_
#define UTILS_HELLO_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/string.h>

/**
* @fn    decir_hola
* @brief Imprime un saludo al nombre que se pase por parámetro por consola.
*/
void decir_hola(char* quien);

//VARIABLES GENERALES
t_log* logger;
t_log* debug_log;
t_config* config;

#endif
