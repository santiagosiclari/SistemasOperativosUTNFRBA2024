#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>

#include <main.h>

int main(int argc, char* argv[]) {
    decir_hola("CPU");
    return 0;


cpu_logger = log_create(,,,LOG_LEVEL_INFO);
if(cpu_logger == NULL){
    perror("Algo raro paso con el log. No se pudo crear o encontrar el archivo.");
    exit(EXIT_FAILURE);
}

cpu_debug = log_create(,,,LOG_LEVEL_TRACE);
if(cpu_debug == NULL){
    perror("Algo raro paso con el log. No se pudo crear o encontrar el archivo.");
    exit(EXIT_FAILURE);
}

cpu_config = config_create("/home/utnso/tp-2024-1c-ChacoForSystem/cpu/cpu.config");
if (cpu_config == NULL)
{
    perror("Error al intertar cargar el config.");
    exit(EXIT_FAILURE);
}

//lectura archivo config
IP_MEMORIA = config_get_string_value(cpu_config, "IP_MEMORIA");
PUERTO_MEMORIA = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
PUERTO_ESCUCHA_DISPATCH = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");
CANTIDAD_ENTRADAS_TLB = config_get_int_value(cpu_config, "CANTIDAD_ENTRADAS_TLB");
ALGORITMO_TLB = config_get_string_value(cpu_config, "ALGORITMO_TLB");


}