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
    fd_kernel = esperar_cliente(memoria_logger, fd_memoria, "Kernel");

    // Espera conexion de CPU
    log_info(memoria_logger, "Esperando al modulo CPU");
    fd_cpu = esperar_cliente(memoria_logger, fd_memoria, "CPU");

    //ESPERAR CONEXION I/O
    //fd_entradasalida = esperar_cliente(memoria_logger, fd_memoria, "I/O");


    return 0;
}