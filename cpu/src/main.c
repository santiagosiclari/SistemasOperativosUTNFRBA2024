#include "../include/main.h"

int main(int argc, char* argv[]) {
    
    // Inicializamos logger y logger debug
    init_cpu_logs();

    // Inicializamos config
    init_cpu_config();

    // Conexiones

    //  Inicio servidor de CPU - dispatch
    fd_cpu_dispatch = iniciar_servidor(cpu_logger,PUERTO_ESCUCHA_DISPATCH,"CPU dispatch inicializado");

    //  Inicio servidor de CPU - interrupt
    fd_cpu_interrupt = iniciar_servidor(cpu_logger,PUERTO_ESCUCHA_INTERRUPT,"CPU interrupt inicializada");

    //  Espera conexiones de Kernel - dispatch
    log_info(cpu_logger,"Esperando Modulo Kernel Dispatch");
    fd_kernel_dispatch = esperar_cliente(cpu_logger, fd_cpu_dispatch, "Kernel Dispatch");

    //  Espera conexiones de Kernel - dispatch
    log_info(cpu_logger,"Esperando Modulo Kernel Interrupt");
    fd_kernel_interrupt = esperar_cliente(cpu_logger, fd_cpu_interrupt, "Kernel Interrupt");

    // Conectar con la memoria
    fd_memoria = crear_conexion(cpu_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");

    // Hilos de Kernel Dispatch - CPU
    pthread_t hilo_kernel_dispatch;
    pthread_create(&hilo_kernel_dispatch, NULL, (void *)conexion_cpu_kernel_dispatch, NULL);
    pthread_join(hilo_kernel_dispatch, NULL);

    return 0;
}