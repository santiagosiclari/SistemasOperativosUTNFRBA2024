#include "../include/main.h"

int main(int argc, char* argv[]) {
    
    // Inicializamos logger y logger debug
    init_cpu_logs();

    // Inicializamos config
    init_cpu_config();

    // Conexiones
    //  Inicio servidor de CPU - dispatch
    fd_cpu_dispatch = iniciar_servidor(cpu_logger,PUERTO_ESCUCHA_DISPATCH,"CPU - Dispatch");

    //  Inicio servidor de CPU - interrupt
    fd_cpu_interrupt = iniciar_servidor(cpu_logger,PUERTO_ESCUCHA_INTERRUPT,"CPU - Interrupt");

    //  Espera conexiones de Kernel - dispatch
    log_info(cpu_logger,"Esperando Modulo Kernel - Dispatch");
    fd_kernel_dispatch = esperar_cliente(cpu_logger, fd_cpu_dispatch, "Kernel - Dispatch");
    if (recv_handshake(fd_kernel_dispatch, HANDSHAKE_CPU_DISPATCH)) {
        log_info(cpu_logger, "Handshake OK de %s", "Kernel/CPU (Dispatch)");
    } else {
        log_error(cpu_logger, "Handshake ERROR de %s", "Kernel/CPU (Dispatch)");
    }

    //  Espera conexiones de Kernel - dispatch
    log_info(cpu_logger,"Esperando Modulo Kernel - Interrupt");
    fd_kernel_interrupt = esperar_cliente(cpu_logger, fd_cpu_interrupt, "Kernel - Interrupt");
    if (recv_handshake(fd_kernel_interrupt, HANDSHAKE_CPU_INTERRUPT)) {
        log_info(cpu_logger, "Handshake OK de %s", "Kernel/CPU (Interrupt)");
    } else {
        log_error(cpu_logger, "Handshake ERROR de %s", "Kernel/CPU (Interrupt)");
    }

    // Conectar con la memoria
    fd_memoria = crear_conexion(cpu_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");
    send_handshake(cpu_logger, fd_memoria, HANDSHAKE_MEMORIA, "CPU/Memoria");

    return 0;
}