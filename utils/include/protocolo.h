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

// PC -> Program Counter
t_buffer* serializar_pc(uint32_t pc);
uint32_t deserializar_pc(t_buffer *buffer, uint32_t pc);
void send_pc(int fd, uint32_t pc);
bool recv_pc(int fd, uint32_t pc);

// Serializacion generica de uint8
t_buffer* serializar_uint8(uint8_t entero);
uint8_t deserializar_uint8(t_buffer *buffer, uint8_t entero);
// send y recv de pid
void send_pid(int fd, uint8_t pid);
bool recv_pid(int fd, uint8_t pid);
// send y recv de size instrucciones
void send_size_instrucciones(int fd, uint8_t size);
bool recv_size_instrucciones(int fd, uint8_t size);

// PCB
t_buffer* serializar_pcb(t_pcb* pcb);
t_pcb* deserializar_pcb(t_buffer *buffer, t_pcb* pcb);
void send_pcb(int fd, t_pcb* pcb);
bool recv_pcb(int fd, t_pcb* pcb);

// Contexto de ejecucion
t_buffer* serializar_contexto_de_ejecucion(t_registros* registros, uint32_t pc);
t_registros* deserializar_registros(t_buffer *buffer, t_registros* registros);
// Funcion de deserializar PC ya hecha
void send_contexto_de_ejecucion(int fd, t_registros* registros, uint32_t pc);
bool recv_contexto_de_ejecucion(int fd, t_registros* registros, uint32_t pc);

// Serializacion de string --> para instruccion, path, etc.
t_buffer* serializar_string(char* string, uint32_t length);
char* deserializar_string(t_buffer *buffer);

// INICIAR_PROCESO
void send_iniciar_proceso(int fd, char* path, uint32_t length);
bool recv_iniciar_proceso(int fd, char* path);

// Instruccion
void send_instruccion(int fd, char* instruccion, uint32_t length);
bool recv_instruccion(int fd, char* instruccion);

#endif