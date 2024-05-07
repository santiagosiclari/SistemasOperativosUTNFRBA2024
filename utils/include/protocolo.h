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

#include "../../utils/include/pcb.h"

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
    // Ejemplo: INICIAR_PROCESO_KM
    
    // ESTAS SON LAS OPERACIONES QUE PENSAMOS QUE VAN A HABER EN NUESTRO TP, PERO PUEDEN IR CAMBIANDO A LO LARGO DEL MISMO

    // Kernel -> Consola interactiva
    // EJECUTAR_SCRIPT 
    INICIAR_PROCESO, // Envia el path del proceso a Memoria
    // FINALIZAR_PROCESO
    // DETENER_PLANIFICACION
    // INICIAR_PLANIFICACION
    // MULTIPROGRAMACION
    // PROCESO_ESTADO

    // CPU (no hace falta ponerlas aca despues las sacamos)
    RECIBIR_PCB,
    // SET,
    // MOV_IN,
    // MOV_OUT,
    // SUM,
    // SUB,
    // JNZ,
    // RESIZE,
    // COPY_STRING,
    // WAIT,
    // SIGNAL,
    // EXIT,

    // Memoria
    RECIBIR_PC, // Recibe la instruccion a enviar para CPU
    // INICIAR_PROCESO_K_M
    // FINALIZAR_PROCESO_K_M
    // AMPLIAR_PROCESO
    // REDUCIR_PROCESO
    // ACCERDER_ESPACIO_USUARIO_K_M
    // ACCERDER_ESPACIO_USUARIO_IO_M

    // EntradaSalida (IO)
    // IO_GEN_SLEEP,
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
typedef struct {
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

// Handshake
void send_handshake(t_log* logger, int fd, int32_t handshake, const char* connection_name);
bool recv_handshake(int fd, int32_t handshakeModulo);

// PCB
bool send_pcb(int fd, t_pcb* pcb);
bool recv_pcb(int fd, t_pcb** pcb);

// Contexto de ejecucion
bool send_contexto_de_ejecucion(int fd, t_registros* registros, uint32_t pc); // Hay q ver si agregar mas cosas como el PATH del proceso
bool recv_contexto_de_ejecucion(int fd, t_registros** registros, uint32_t* pc);

// Char* (String) --> para instrucciones
bool send_string(int fd, char* string);
bool recv_string(int fd, char** string);

// PC -> Program Counter
bool send_pc(int fd, uint32_t pc);
bool recv_pc(int fd, uint32_t* pc);

// PID
bool send_pid(int fd, uint8_t pid);
bool recv_pid(int fd, uint8_t* pid);

// INICIAR_PROCESO
bool send_iniciar_proceso(int fd, char* path);
bool recv_iniciar_proceso(int fd, char** path);

#endif