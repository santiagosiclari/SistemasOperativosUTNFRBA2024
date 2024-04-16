#include "../include/main.h"

int main(int argc, char* argv[]) {

    // Inicializamos logger y logger debug
    init_kernel_logs();
    // Inicializamos config
    init_kernel_config();

    //  Conectamos como cliente con el modulo CPU - Dispatch
    fd_cpu_dispatch = crear_conexion(kernel_logger, IP_CPU, PUERTO_CPU_DISPATCH, "Cpu - Dispatch");
    send_handshake(kernel_logger, fd_cpu_dispatch, HANDSHAKE_CPU_DISPATCH, "Kernel/CPU (Dispatch)");

    // Conectamos como cliente con el modulo CPU - Interrupt
    fd_cpu_interrupt = crear_conexion(kernel_logger, IP_CPU, PUERTO_CPU_INTERRUPT,"Cpu - Interrupt");
    send_handshake(kernel_logger, fd_cpu_interrupt, HANDSHAKE_CPU_INTERRUPT, "Kernel/CPU (Interrupt)");

    // Conectamos como cliente con el modulo MEMORIA
    fd_memoria = crear_conexion(kernel_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");
    send_handshake(kernel_logger, fd_memoria, HANDSHAKE_MEMORIA, "Kernel/Memoria");

    // Iniciar servidor
    fd_kernel = iniciar_servidor(kernel_logger, PUERTO_ESCUCHA, "Kernel");

    //Conectamos como servidor con el modulo I/O
    log_info(kernel_logger, "Esperando al modulo Entrada Salida");
    fd_entradasalida = esperar_cliente(kernel_logger, fd_kernel, "Entrada Salida");
    if (recv_handshake(fd_entradasalida, HANDSHAKE_KERNEL)) {
        log_info(kernel_logger, "Handshake OK de %s", "IO/Kernel");
    } else {
        log_error(kernel_logger, "Handshake ERROR de %s", "IO/Kernel");
    }

    // Todavia no tiene ninguna funcionalidad pero ya esta inicializada
    iniciar_consola(kernel_logger);
    
    // Terminar programa
    liberar_conexion(fd_cpu_dispatch);
    liberar_conexion(fd_cpu_interrupt);
    liberar_conexion(fd_memoria);
    liberar_conexion(fd_entradasalida);
    terminar_programa(fd_kernel, kernel_logger, kernel_config);

    return 0;
}