#include "../include/main.h"

int main(int argc, char* argv[]) {

    // Inicializamos logger y logger debug
    init_entradasalida_logs();

    // Inicializamos config
    init_entradasalida_config();

    // Conexiones
    // Conexion con Kernel
    fd_kernel = crear_conexion(entradasalida_logger, IP_KERNEL, PUERTO_KERNEL, "Kernel");
    send_handshake(entradasalida_logger, fd_kernel, HANDSHAKE_KERNEL, "IO/Kernel");

    // Hilos
    pthread_t entradasalida_kernel;
    pthread_create(&entradasalida_kernel, NULL, (void *)conexion_entradasalida_kernel, NULL);
    pthread_detach(entradasalida_kernel);

    // Pedir nombre y archivo config
    log_info(entradasalida_logger, "Ingrese el nombre de la I/O:");
    char* nombre;
	nombre = readline("> ");
    log_info(entradasalida_logger, "Nombre de la I/O leido: %s\n", nombre);
	
    log_info(entradasalida_logger, "Ingrese el archivo de configuracion:"); // ./memoria.config
    char* config;
	config = readline("> ");
    log_info(entradasalida_logger, "Archivo de config leido: %s\n", config);

    // Conexion con Memoria
    fd_memoria = crear_conexion(entradasalida_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");
    send_handshake(entradasalida_logger, fd_memoria, HANDSHAKE_MEMORIA, "IO/Memoria");

    // Hilos
    pthread_t entradasalida_memoria;
    pthread_create(&entradasalida_memoria, NULL, (void *)conexion_entradasalida_memoria, NULL);
    pthread_join(entradasalida_memoria, NULL);

    // Terminar programa
    free(nombre);
    free(config);
    terminar_entradasalida();

    return 0;
}