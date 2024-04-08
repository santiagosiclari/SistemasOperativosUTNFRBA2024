#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include "main.h"

int main(int argc, char* argv[]) {
    decir_hola("Kernel");

kernel_logger = log_create(,,,LOG_LEVEL_INFO);
if(kernel_logger == NULL){
    perror("Algo raro paso con el log. No se pudo crear o encontrar el archivo.");
    exit(EXIT_FAILURE);
}

kernel_debug = log_create(,,,LOG_LEVEL_TRACE);
if(kernel_debug == NULL){
    perror("Algo raro paso con el log. No se pudo crear o encontrar el archivo.");
    exit(EXIT_FAILURE);
}

kernel_config = config_create("/home/utnso/tp-2024-1c-ChacoForSystem/kernell/kernel.config");
if (kernel_config == NULL)
{
    perror("Error al intertar cargar el config.");
    exit(EXIT_FAILURE);
}

    //leer de archivo config
    PUERTO_ESCUCHA = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");
    IP_MEMORIA = config_get_string_value(kernel_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_int_value(kernel_config, "PUERTO_MEMORIA");
    IP_CPU = config_get_string_value(kernel_config, "IP_CPU");
    PUERTO_CPU_DISPATCH = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
    PUERTO_CPU_INTERRUPT = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
    ALGORITMO_PLANIFICACION = config_get_string_value(kernel_config, "ALGORITMO_PLANIFICACION");
    QUANTUM = config_get_int_value(kernel_config, "QUANTUM");
    RECURSOS = config_get_array_value(kernel_config, "RECUERSOS");
    INSTANCIAS_RECURSOS = config_get_array_value(kernel_config, "INSTANCIAS_RECUERSOS");
    GRADO_MULTIPROGRAMACION = config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");



return 0;
}