#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <stdio.h>
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

// Definir funciones de send y recv de cada operacion

#endif