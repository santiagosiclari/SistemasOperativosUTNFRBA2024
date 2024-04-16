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


} op_code;

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

extern int errno;

// Definir funciones de send y recv de cada operacion
bool send_mensaje(int fd, uint8_t num);
bool recv_mensaje(int fd, uint8_t* num);
bool send_handshake(t_log* logger,int fd,int32_t hs,const char* connection_name);
bool recv_handshake(int fd);

#endif