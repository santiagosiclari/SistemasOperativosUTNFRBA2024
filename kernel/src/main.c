#include "../include/main.h"

int main(int argc, char* argv[]) {

    // Inicializamos logger y logger debug
    init_kernel_logs();
    // Inicializamos config
    init_kernel_config();

    //  Conectamos como cliente con el modulo CPU - Dispatch
    fd_cpu_dispatch = crear_conexion(kernel_logger,IP_CPU,PUERTO_CPU_DISPATCH, "Cpu - Dispatch");

    // Conectamos como cliente con el modulo CPU - Interrupt
    fd_cpu_interrupt = crear_conexion(kernel_logger,IP_CPU,PUERTO_CPU_INTERRUPT,"Cpu - Interrupt");

    // Conectamos como cliente con el modulo MEMORIA
    fd_memoria = crear_conexion(kernel_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");

    return 0;
}