#include "../include/main.h"

int main(int argc, char* argv[]) {
    // Inicializamos logger y logger debug
    init_entradasalida_logs();

    // Inicializamos config
    init_entradasalida_config();

    // Conexiones
    // Conexion con Kernel
    fd_kernel = crear_conexion(entradasalida_logger, IP_KERNEL, PUERTO_KERNEL, "Kernel");
    send_handshake(entradasalida_logger, fd_kernel, HANDSHAKE_KERNEL, "IO/Kernel");

    // Conexion con Memoria
    fd_memoria = crear_conexion(entradasalida_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");
    send_handshake(entradasalida_logger, fd_memoria, HANDSHAKE_MEMORIA, "IO/Memoria");

    return 0;
}