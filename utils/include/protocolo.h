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

#include "pcb.h"
#include "funciones-serializacion.h"
#include "file-descriptors.h"

// Leer protocolo.txt en la carpeta utils para saber el inicio de cada modulo

// Handshake
void send_handshake(t_log* logger, int fd, int32_t handshake, const char* connection_name);
bool recv_handshake(int fd, int32_t handshakeModulo);

// PCB
t_buffer* serializar_pcb(t_pcb* pcb);
t_pcb* deserializar_pcb(t_buffer *buffer, t_pcb* pcb);
bool send_pcb(int fd, t_pcb* pcb);
bool recv_pcb(int fd, t_pcb* pcb);

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