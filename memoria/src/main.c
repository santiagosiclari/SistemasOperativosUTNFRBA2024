#include "../include/main.h"

int main(int argc, char* argv[]) {

    // Inicializamos logger y logger debug
    init_memoria_logs();

    // Inicializamos config
    init_memoria_config();

    // Conexiones

    // Iniciar servidor
    fd_memoria = iniciar_servidor(memoria_logger, PUERTO_ESCUCHA, "Memoria");

    // Conexiones
    // Espera conexion de Kernel
    log_info(memoria_logger, "Esperando al modulo Kernel");
    fd_kernel = esperar_cliente(memoria_logger, fd_memoria, "Kernel");
    if (recv_handshake(fd_kernel, HANDSHAKE_MEMORIA)) {
        log_info(memoria_logger, "Handshake OK de %s", "Kernel/Memoria");
    } else {
        log_error(memoria_logger, "Handshake ERROR de %s", "Kernel/Memoria");
    }
    
    // Espera conexion de CPU
    log_info(memoria_logger, "Esperando al modulo CPU");
    fd_cpu = esperar_cliente(memoria_logger, fd_memoria, "CPU");
    if (recv_handshake(fd_cpu, HANDSHAKE_MEMORIA)) {
        log_info(memoria_logger, "Handshake OK de %s", "CPU/Memoria");
    } else {
        log_error(memoria_logger, "Handshake ERROR de %s", "CPU/Memoria");
    }

    // ESPERAR CONEXION I/O
    log_info(memoria_logger, "Esperando al modulo Entrada/Salida");
    fd_entradasalida = esperar_cliente(memoria_logger, fd_memoria, "I/O");
    if (recv_handshake(fd_entradasalida, HANDSHAKE_MEMORIA)) {
        log_info(memoria_logger, "Handshake OK de %s", "IO/Memoria");
    } else {
        log_error(memoria_logger, "Handshake ERROR de %s", "IO/Memoria");
    }

    // Terminar programa
    liberar_conexion(fd_kernel);
    liberar_conexion(fd_cpu);
    liberar_conexion(fd_entradasalida);
    terminar_programa(fd_memoria, memoria_logger, memoria_config);

    return 0;
}