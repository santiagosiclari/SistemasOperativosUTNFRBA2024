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
    // Espera conexion de CPU
    log_info(memoria_logger, "Esperando al modulo CPU");
    fd_cpu = esperar_cliente(memoria_logger, fd_memoria, "CPU");
    if (recv_handshake(fd_cpu, HANDSHAKE_MEMORIA)) {
        log_info(memoria_logger, "Handshake OK de %s\n", "CPU/Memoria");
    } else {
        log_error(memoria_logger, "Handshake ERROR de %s\n", "CPU/Memoria");
    }

    // Hilos
    pthread_t memoria_cpu;
    pthread_create(&memoria_cpu, NULL, (void *)conexion_memoria_cpu, NULL);
    pthread_detach(memoria_cpu);

    // Espera conexion de Kernel
    log_info(memoria_logger, "Esperando al modulo Kernel");
    fd_kernel = esperar_cliente(memoria_logger, fd_memoria, "Kernel");
    if (recv_handshake(fd_kernel, HANDSHAKE_MEMORIA)) {
        log_info(memoria_logger, "Handshake OK de %s\n", "Kernel/Memoria");
    } else {
        log_error(memoria_logger, "Handshake ERROR de %s\n", "Kernel/Memoria");
    }

    // Hilos
    pthread_t memoria_kernel;
    pthread_create(&memoria_kernel, NULL, (void *)conexion_memoria_kernel, NULL);
    pthread_detach(memoria_kernel);

    // Multiples interfaces I/O
    while (1) {
        // ESPERAR CONEXION I/O
        log_info(memoria_logger, "Esperando al modulo Entrada/Salida");
        fd_entradasalida = esperar_cliente(memoria_logger, fd_memoria, "I/O");
        if (recv_handshake(fd_entradasalida, HANDSHAKE_MEMORIA)) {
            log_info(memoria_logger, "Handshake OK de %s\n", "IO/Memoria");
        } else {
            log_error(memoria_logger, "Handshake ERROR de %s\n", "IO/Memoria");
        }

        // Hilos
        pthread_t memoria_entradasalida;
        pthread_create(&memoria_entradasalida, NULL, (void *)conexion_memoria_entradasalida, NULL);
        pthread_detach(memoria_entradasalida);
    }

    // Terminar programa
    terminar_memoria();

    return 0;
}