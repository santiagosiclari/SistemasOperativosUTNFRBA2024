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

// Serializacion generica de uint32
t_buffer* serializar_uint32(uint32_t entero);
uint32_t deserializar_uint32(t_buffer* buffer, uint32_t entero);

// Serializacion generica de uint8
t_buffer* serializar_uint8(uint8_t entero);
uint8_t deserializar_uint8(t_buffer *buffer, uint8_t entero);

// send y recv de pid
void send_pid(int fd, uint8_t pid);
bool recv_pid(int fd, uint8_t pid);

// PID a borrar (EXIT)
void send_pid_a_borrar(int fd, uint8_t pid_a_borrar);
bool recv_pid_a_borrar(int fd, uint8_t pid_a_borrar);

// send y recv pc y pid
t_buffer* serializar_pc_pid(uint32_t pc, uint8_t pid);
void send_pc_pid(int fd, uint32_t pc, uint8_t pid);
bool recv_pc_pid(int fd, uint32_t pc, uint8_t pid);

// send y recv de size instrucciones
void send_size_instrucciones(int fd, uint8_t size);
bool recv_size_instrucciones(int fd, uint8_t size);

// PCB --> tambien contexto de ejecucion
t_buffer* serializar_pcb(t_pcb* pcb);
t_pcb* deserializar_pcb(t_buffer *buffer, t_pcb* pcb);
void send_pcb(int fd, t_pcb* pcb);
bool recv_pcb(int fd, t_pcb* pcb);

// Serializacion de string --> para INICIAR_PROCESO, instrucciones, etc.
t_buffer* serializar_string(char* string, uint32_t length);
char* deserializar_string(t_buffer *buffer);

// INICIAR_PROCESO
t_buffer* serializar_iniciar_proceso(uint8_t pid, char* string, uint32_t length);
void send_iniciar_proceso(int fd, uint8_t pid, char* path, uint32_t length);
bool recv_iniciar_proceso(int fd, uint8_t pid, char* path);

// Instruccion
void send_instruccion(int fd, char* instruccion, uint32_t length);
bool recv_instruccion(int fd, char* instruccion);

// IOs
// Finalizar IO
void send_fin_io(int fd, char* nombre, uint32_t length);
bool recv_fin_io(int fd, char* nombre);

// IO_GEN_SLEEP
t_buffer* serializar_io_gen_sleep(uint32_t sleep, char* string, uint32_t length);
void send_io_gen_sleep(int fd, uint32_t unidades_trabajo, char* nombre, uint32_t length);
bool recv_io_gen_sleep(int fd, uint32_t unidades_trabajo, char* nombre);

#endif