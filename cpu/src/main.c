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

    while (1) {
        // Usar semaforos        
        // Ciclo de instruccion
        char* instruccion;
        char** instruccion_separada;
        uint8_t pid_interrumpido;
        while (!recv_pid(fd_kernel_interrupt, &pid_interrumpido) && pid_interrumpido != pcb->pid) { // Revisar para ver si se sale del while
            // Fetch
            // Pedir instruccion
            if(!send_pc(fd_memoria, pcb->pc)) {
                log_error(cpu_logger, "Hubo un error al enviar el PC (Program Counter) al modulo de Memoria");
            }
            
            // Recibir la instruccion
            if (!recv_string(fd_memoria, &instruccion)) {
                log_error(cpu_logger, "Hubo un error al recibir la instruccion del modulo de Memoria");
            }

            log_info(cpu_logger, "Iniciando instruccion: %d", pcb->pc); // aca pasariamos el PC del PCB

            // Decode
            instruccion_separada = string_split(instruccion, " ");
            if (strcmp(instruccion_separada[0], "SET") == 0) {
                pcb = cpu_set(pcb, instruccion_separada);
            } else if (strcmp(instruccion_separada[0], "SUM") == 0) {
                pcb = cpu_sum(pcb, instruccion_separada);
            } else if (strcmp(instruccion_separada[0], "SUB") == 0) {
                pcb = cpu_sub(pcb, instruccion_separada);
            } else if (strcmp(instruccion_separada[0], "JNZ") == 0) {
                pcb = cpu_jnz(pcb, instruccion_separada);
            } else if (strcmp(instruccion_separada[0], "IO_GEN_SLEEP") == 0) {
                pcb = cpu_io_gen_sleep(pcb, instruccion_separada);
            } else {
                log_warning(cpu_logger, "No se pudo encontrar la instruccion\n");
            }
        }

        // Check Interrupt
        if (pid_interrumpido == pcb->pid) {
            // FALTA hacer funcionalidad de interrupcion
            // Solo envia un paquete por kernel dispatch con el contexto de ejecucion (CREO)

            
        }

        free(instruccion);
        free(instruccion_separada);
    }

    // Terminar programa
    terminar_cpu();

    return 0;
}