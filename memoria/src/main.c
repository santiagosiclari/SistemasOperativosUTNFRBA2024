#include "../include/main.h"

int main(int argc, char* argv[]) {
    
    // Inicializamos logger y logger debug
    init_memoria_logs();

    // Inicializamos config
    init_memoria_config();

    // Iniciar servidor de memoria
    int fd_memoria;
    fd_memoria = iniciar_servidor(memoria_logger, NULL, PUERTO_ESCUCHA, "Memoria");
    log_info(memoria_logger, "Socket memoria: %d", fd_memoria); // Prueba

    // Conexiones
    // Espera conexion de Kernel


    // Espera conexion de EntradaSalida


    // Espera conexion de CPU
    log_info(memoria_logger, "Esperando al modulo CPU");
    int fd_cpu;
    fd_cpu = esperar_cliente(memoria_logger, fd_memoria, "CPU");
    log_info(memoria_logger, "Socket cpu: %d", fd_cpu); // Prueba

    

    return 0;
}