#include <../include/configs.h>

void init_entradasalida_config(void) {
    entradasalida_config = config_create("/home/utnso/tp-2024-1c-ChacoForSystem/entradasalida/entradasalida.config");
    if (entradasalida_config == NULL) {
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
}