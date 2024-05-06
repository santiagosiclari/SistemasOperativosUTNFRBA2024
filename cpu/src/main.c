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

    // Conectar con la memoria
    fd_memoria = crear_conexion(cpu_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");
    send_handshake(cpu_logger, fd_memoria, HANDSHAKE_MEMORIA, "CPU/Memoria");

    while (1) {
        //  Espera conexiones de Kernel - dispatch
        log_info(cpu_logger,"Esperando Modulo Kernel - Dispatch");
        fd_kernel_dispatch = esperar_cliente(cpu_logger, fd_cpu_dispatch, "Kernel - Dispatch");
        if (recv_handshake(fd_kernel_dispatch, HANDSHAKE_CPU_DISPATCH)) {
            log_info(cpu_logger, "Handshake OK de %s\n", "Kernel/CPU (Dispatch)");
        } else {
            log_error(cpu_logger, "Handshake ERROR de %s\n", "Kernel/CPU (Dispatch)");
        }

        // Hilos
        pthread_t cpu_kernel_dispatch;
        pthread_create(&cpu_kernel_dispatch, NULL, (void *)conexion_cpu_kernel_dispatch, NULL);
        pthread_detach(cpu_kernel_dispatch);

        //  Espera conexiones de Kernel - dispatch
        log_info(cpu_logger,"Esperando Modulo Kernel - Interrupt");
        fd_kernel_interrupt = esperar_cliente(cpu_logger, fd_cpu_interrupt, "Kernel - Interrupt");
        if (recv_handshake(fd_kernel_interrupt, HANDSHAKE_CPU_INTERRUPT)) {
            log_info(cpu_logger, "Handshake OK de %s\n", "Kernel/CPU (Interrupt)");
        } else {
            log_error(cpu_logger, "Handshake ERROR de %s\n", "Kernel/CPU (Interrupt)");
        }

        // Hilos
        pthread_t cpu_kernel_interrupt;
        pthread_create(&cpu_kernel_interrupt, NULL, (void *)conexion_cpu_kernel_interrupt, NULL);
        pthread_detach(cpu_kernel_interrupt);
        
        // Fetch
        // if (recv_iniciar_proceso) {
        //     // Usar semaforos
        //     leer_archivo();
        // }
    }

    // Terminar programa
    terminar_cpu();

    return 0;
}