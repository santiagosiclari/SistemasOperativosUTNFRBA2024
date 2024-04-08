#include "../include/main.h"

int main(int argc, char* argv[]) {
    //decir_hola("una Interfaz de Entrada/Salida");

entradasalida_logger = log_create(,,,LOG_LEVEL_INFO);
if(entradasalida_logger == NULL){
    perror("Algo raro paso con el log. No se pudo crear o encontrar el archivo.");
    exit(EXIT_FAILURE);
}

entradasalida_debug = log_create(,,,LOG_LEVEL_TRACE);
if(entradasalida_debug == NULL){
    perror("Algo raro paso con el log. No se pudo crear o encontrar el archivo.");
    exit(EXIT_FAILURE);
}

entradasalida_config = config_create("/home/utnso/tp-2024-1c-ChacoForSystem/entradasalida/entradasalida.config");
if (entradasalida_config == NULL)
{
    perror("Error al intertar cargar el config.");
    exit(EXIT_FAILURE);
}

TIPO_INTERFAZ = config_get_string_value(entradasalida_config, "TIPO_INTERFAZ");
TIEMPO_UNIDAD_TRABAJO = config_get_int_value(entradasalida_config, "TIEMPO_UNIDAD_TRABAJO");
IP_KERNEL = config_get_string_value(entradasalida_config, "IP_KERNEL");
PUERTO_KERNEL = config_get_string_value(entradasalida_config, "PUERTO_KERNEL");
IP_MEMORIA = config_get_string_value(entradasalida_config, "IP_MEMORIA");
PUERTO_MEMORIA = config_get_string_value(entradasalida_config, "PUERTO_MEMORIA");
PATH_BASE_DIALFS = config_get_string_value(entradasalida_config, "PATH_BASE_DIALFS");
BLOCK_SIZE = config_get_int_value(entradasalida_config, "BLOCK_SIZE");
BLOCK_COUNT = config_get_int_value(entradasalida_config, "BLOCK_COUNT");

return 0;
}