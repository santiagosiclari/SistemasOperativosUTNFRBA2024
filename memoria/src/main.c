#include "../include/main.h"

int main(int argc, char* argv[]) {

    // Inicializamos logger y logger debug
    init_memoria_logs();

    // Inicializamos config
    init_memoria_config();

    // Conexiones

    //iniciar servidor
    fd_memoria = iniciar_servidor(memoria_logger, PUERTO_ESCUCHA, "Memoria iniciada");

    // Conexiones
    // Espera conexion de Kernel
    log_info(memoria_logger, "Esperando al modulo Kernel");
    fd_kernel = esperar_cliente(memoria_logger, fd_memoria, "Kernel");

    // Espera conexion de CPU
    log_info(memoria_logger, "Esperando al modulo CPU");
    fd_cpu = esperar_cliente(memoria_logger, fd_memoria, "CPU");

    //ESPERAR CONEXION I/O
    //fd_entradasalida = esperar_cliente(memoria_logger, fd_memoria, "I/O");

    // Hilos Memoria - Kernel
    pthread_t hilo_memoria;
    pthread_create(&hilo_memoria, NULL, (void*)conexion_memoria_kernel, NULL);
    pthread_join(hilo_memoria, NULL);

    return 0;
}