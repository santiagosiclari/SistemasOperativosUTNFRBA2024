#include "../include/main.h"

int main(int argc, char* argv[]) {
    
    // Inicializamos logger y logger debug
    init_memoria_logs();

    // Inicializamos config
    init_memoria_config();

    // Iniciar servidor de memoria
    fd_memoria = iniciar_servidor(memoria_logger, PUERTO_ESCUCHA, "Memoria");

    // Conexiones
    // Espera conexion de Kernel


    // Espera conexion de EntradaSalida


    // Espera conexion de CPU
    log_info(memoria_logger, "Esperando al modulo CPU");
    fd_cpu = esperar_cliente(memoria_logger, fd_memoria, "CPU");

    

    return 0;
}