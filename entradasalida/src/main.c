#include "../include/main.h"

int main(int argc, char* argv[]) {
    // Inicializamos logger y logger debug
    init_entradasalida_logs();

    // Inicializamos config
    init_entradasalida_config();

    // Conexiones
    fd_kernel = crear_conexion(entradasalida_logger, IP_KERNEL, PUERTO_KERNEL, "KERNEL");

    fd_memoria = crear_conexion(entradasalida_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");


    return 0;
}