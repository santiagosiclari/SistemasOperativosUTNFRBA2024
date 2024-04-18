#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <inttypes.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <commons/log.h>
#include <sys/socket.h>

// Leer protocolo.txt en la carpeta utils para saber el inicio de cada modulo

typedef enum {
	MENSAJE,
	PAQUETE,

    // Se van a crear mas operaciones segun el enunciado
    // Para organizar mejor vamos a definirlos asi: Nombre_ModuloQueEnvia_ModuloQueRecibe,
    // La abreviatura de los modulos son:
    // - Kernel = K
    // - CPU = CPU
    // - Memoria = M
    // - Entrada/Salida = IO
    // Ejemplo: INICIAR_PROCESO_K_CPU
    
    // ESTAS SON LAS OPERACIONES QUE PENSAMOS QUE VAN A HABER EN NUESTRO TP, PERO PUEDEN IR CAMBIANDO A LO LARGO DEL MISMO

    // Kernel
    // EJECUTAR_SCRIPT 
    // INICIAR_PROCESO
    // FINALIZAR_PROCESO
    // DETENER_PLANIFICACION
    // INICIAR_PLANIFICACION
    // MULTIPROGRAMACION
    // PROCESO_ESTADO


    // CPU
    // SET
    // MOV_IN
    // MOV_OUT
    // SUM
    // SUB
    // JNZ
    // RESIZE
    // COPY_STRING
    // WAIT
    // SIGNAL
    // EXIT


    // Memoria
    // INICIAR_PROCESO_K_M
    // FINALIZAR_PROCESO_K_M
    // AMPLIAR_PROCESO
    // REDUCIR_PROCESO
    // ACCERDER_ESPACIO_USUARIO_K_M
    // ACCERDER_ESPACIO_USUARIO_IO_M

    // EntradaSalida
    // IO_GEN_SLEEP
    // IO_STDIN_READ
    // IO_STDOUT_WRITE
    // IO_FS_CREATE
    // IO_FS_DELETE
    // IO_FS_TRUNCATE
    // IO_FS_WRITE
    // IO_FS_READ

} op_code;

typedef enum {
    HANDSHAKE_KERNEL,
    HANDSHAKE_CPU_DISPATCH,
    HANDSHAKE_CPU_INTERRUPT,
    HANDSHAKE_MEMORIA,
} op_handshake;

// Estrucuta del buffer
typedef struct {
	int size;
	void* stream;
} t_buffer;

// Estructura del paquete
typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

// Handshake
bool send_handshake(t_log* logger, int fd, int32_t handshake, const char* connection_name);
bool recv_handshake(int fd, int32_t handshakeModulo);

#endif