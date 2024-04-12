#include "../include/main.h"

int main(int argc, char* argv[]) {

    // Inicializamos logger y logger debug
    init_kernel_logs();
    // Inicializamos config
    init_kernel_config();

    // Conectamos como cliente con el modulo MEMORIA
    fd_memoria = crear_conexion(kernel_logger, IP_MEMORIA, PUERTO_MEMORIA, "CONEXION CREADA KERNEL - MEMORIA");

    return 0;
}