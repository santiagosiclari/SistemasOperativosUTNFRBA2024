#include "../include/main.h"

// Variable global
char* nombre;
// char* config;

int main(int argc, char* argv[]) {

    // Inicializamos logger y logger debug
    init_entradasalida_logs();

    // Pedir nombre y archivo config
    log_info(entradasalida_logger, "Ingrese el nombre de la I/O:");
	nombre = readline("> ");
    log_info(entradasalida_logger, "Nombre de la I/O leido: %s\n", nombre);
	
    // log_info(entradasalida_logger, "Ingrese el archivo de configuracion:"); // ./memoria.config
	// config = readline("> ");
    // log_info(entradasalida_logger, "Archivo de config leido: %s\n", config);

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

    // Conexion con Memoria
    fd_memoria = crear_conexion(entradasalida_logger, IP_MEMORIA, PUERTO_MEMORIA, "Memoria");
    send_handshake(entradasalida_logger, fd_memoria, HANDSHAKE_MEMORIA, "IO/Memoria");

    // Envia a Kernel y Memoria el nombre
    send_interfaz(fd_kernel, nombre, strlen(nombre) + 1);
    send_interfaz(fd_memoria, nombre, strlen(nombre) + 1);

    // Hilos
    pthread_t entradasalida_memoria;
    pthread_create(&entradasalida_memoria, NULL, (void *)conexion_entradasalida_memoria, NULL);
    pthread_join(entradasalida_memoria, NULL);

    // Terminar programa
    free(nombre);
    // free(config);
    terminar_entradasalida();

    return 0;
}