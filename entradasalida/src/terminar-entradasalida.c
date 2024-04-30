#include "../include/terminar-entradasalida.h"

void terminar_entradasalida() {
    log_destroy(entradasalida_logger);
    config_destroy(entradasalida_config);
    liberar_conexion(fd_kernel);
    liberar_conexion(fd_memoria);
}