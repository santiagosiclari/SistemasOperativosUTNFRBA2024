#include "../include/terminar-memoria.h"

void terminar_memoria() {
    log_destroy(memoria_logger);
    config_destroy(memoria_config);
    liberar_conexion(fd_kernel);
    liberar_conexion(fd_cpu);
    liberar_conexion(fd_entradasalida);
}