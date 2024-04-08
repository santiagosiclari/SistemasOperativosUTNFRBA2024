#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>

typedef enum {
	MENSAJE,
	PAQUETE,
    // Se van a crear mas operaciones segun el protocolo de comunicacion
} op_code;

typedef struct {
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

int crear_conexion(t_log* logger, char *ip, char* puerto, char* modulo);
int iniciar_servidor(t_log* logger, char* ip, char* puerto, char* modulo);
int esperar_cliente(t_log* logger, int socket_servidor, char* modulo);
int recibir_operacion(t_log* logger, int socket_cliente, char* ip, char* puerto, char* modulo);
void liberar_conexion(int socket_cliente);

#endif