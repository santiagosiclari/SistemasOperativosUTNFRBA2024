#include "../include/main.h"

int main(int argc, char* argv[]) {

    // Inicializamos logger y logger debug
    init_memoria_logs();

    // Inicializamos config
    init_memoria_config();

    // Conexiones

    //iniciar servidor
    fd_memoria = iniciar_servidor(memoria_logger, PUERTO_ESCUCHA, "Memoria");

    // Conexiones
    // Espera conexion de Kernel
    log_info(memoria_logger, "Esperando al modulo Kernel");
    fd_kernel = esperar_cliente(memoria_logger, fd_memoria, "Kernel");

    // Espera conexion de CPU
    log_info(memoria_logger, "Esperando al modulo CPU");
    fd_cpu = esperar_cliente(memoria_logger, fd_memoria, "CPU");

    // ESPERAR CONEXION I/O
    log_info(memoria_logger, "Esperando al modulo Entrada/Salida");
    fd_entradasalida = esperar_cliente(memoria_logger, fd_memoria, "I/O");

    // Hilos Kernel - Memoria
    pthread_t hilo_kernel;
    pthread_create(&hilo_kernel, NULL, (void*)conexion_memoria_kernel, NULL);
    pthread_detach(hilo_kernel);

    // Hilos CPU - Memoria
    pthread_t hilo_cpu;
    pthread_create(&hilo_cpu, NULL, (void*)conexion_memoria_cpu, NULL);
    pthread_detach(hilo_cpu);

    return 0;
}