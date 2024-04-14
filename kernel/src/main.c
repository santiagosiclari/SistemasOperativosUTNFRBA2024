#include "../include/main.h"

int main(int argc, char* argv[]) {

    // Inicializamos logger y logger debug
    init_kernel_logs();
    // Inicializamos config
    init_kernel_config();

    //iniciar servidor
    fd_kernel = iniciar_servidor(kernel_logger, PUERTO_ESCUCHA, "kernel iniciada");

    //  Conectamos como cliente con el modulo CPU - Dispatch
    fd_cpu_dispatch = crear_conexion(kernel_logger,IP_CPU,PUERTO_CPU_DISPATCH, "Cpu - Dispatch");

    // Conectamos como cliente con el modulo CPU - Interrupt
    fd_cpu_interrupt = crear_conexion(kernel_logger,IP_CPU,PUERTO_CPU_INTERRUPT,"Cpu - Interrupt");

    // Conectamos como cliente con el modulo MEMORIA
    fd_memoria = crear_conexion(kernel_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");

    //Conectamos como servidor con el modulo I/O
    log_info(kernel_logger, "Esperando al modulo Entrada salida");
    fd_entradasalida = esperar_cliente(kernel_logger, fd_kernel, "Entrada Salida");

    int a;

    scanf("%d", &a);
    send_mensaje(fd_cpu_dispatch, a);

    scanf("%d", &a);
    send_mensaje(fd_memoria, a);

    return 0;
}