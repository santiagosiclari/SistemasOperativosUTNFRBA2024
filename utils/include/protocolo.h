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

// send y recv de pid
void send_pid(int fd, uint8_t pid);
bool recv_pid(int fd, uint8_t* pid);

// PID a borrar (EXIT)
void send_pid_a_borrar(int fd, uint8_t pid_a_borrar);
bool recv_pid_a_borrar(int fd, uint8_t* pid_a_borrar);

// PID a interrumpir
void send_interrupcion(int fd, uint8_t pid_a_interrumpir);
bool recv_interrupcion(int fd, uint8_t* pid_a_interrumpir);

// send y recv pc y pid
t_buffer* serializar_pc_pid(uint32_t pc, uint8_t pid);
void send_pc_pid(int fd, uint32_t pc, uint8_t pid);
bool recv_pc_pid(int fd, uint32_t* pc, uint8_t* pid);

// send y recv de size instrucciones
void send_size_instrucciones(int fd, uint8_t size);
bool recv_size_instrucciones(int fd, uint8_t* size);

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
bool recv_iniciar_proceso(int fd, uint8_t* pid, char* path);

// Instruccion
void send_instruccion(int fd, char* instruccion, uint32_t length);
bool recv_instruccion(int fd, char* instruccion);

// WAIT y SIGNAL
t_buffer* serializar_wait_signal(t_pcb* pcb_wait_signal, char* recurso, uint32_t length);
void send_wait(int fd, t_pcb* pcb_wait, char* recurso, uint32_t length);
void send_signal(int fd, t_pcb* pcb_signal, char* recurso, uint32_t length);
bool recv_wait_signal(int fd, t_pcb* pcb_wait_signal, char* recurso);

// RECURSOS_OK
void send_recursos_ok(int fd, uint8_t recursos_ok);
bool recv_recursos_ok(int fd, uint8_t* recursos_ok);

// TAM_PAGINA
void send_tam_pagina(int fd, uint32_t tam_pagina);
bool recv_tam_pagina(int fd, uint32_t* tam_pagina);

// RESIZE
void send_tamanio(int fd, uint32_t tamanio, uint8_t pid_resize);
bool recv_tamanio(int fd, uint32_t* tamanio, uint8_t* pid_resize);

// Out of Memory
void send_out_of_memory(int fd, uint8_t pid_oom);
bool recv_out_of_memory(int fd, uint8_t* pid_oom);

// Escribir memoria
t_buffer* serializar_escribir_memoria(uint8_t pid_a_escribir, uint32_t direccion_fisica, void* datos, uint32_t tamanio_a_escribir);
void send_escribir_memoria(int fd, uint8_t pid_a_escribir, uint32_t direccion_fisica, void* datos, uint32_t tamanio_a_escribir);
bool recv_escribir_memoria(int fd, uint8_t* pid_a_escribir, uint32_t* direccion_fisica, void** datos, uint32_t* tamanio_a_escribir);

// Leer memoria
t_buffer* serializar_leer_memoria(uint8_t pid_a_leer, uint32_t direccion_fisica, uint32_t tamanio_a_leer);
void send_leer_memoria(int fd, uint8_t pid_a_leer, uint32_t direccion_fisica, uint32_t tamanio_a_leer);
bool recv_leer_memoria(int fd, uint8_t* pid_a_leer, uint32_t* direccion_fisica, uint32_t* tamanio_a_leer);

// Numero de pagina
t_buffer* serializar_pagina_marco(uint8_t pid, uint32_t num_pagina, uint32_t desplazamiento);
void send_num_pagina(int fd, uint8_t pid, uint32_t num_pagina, uint32_t desplazamiento);
bool recv_num_pagina(int fd, uint8_t* pid, uint32_t* num_pagina, uint32_t* desplazamiento);

// RECIBIR_MARCO
void send_num_marco(int fd, uint8_t pid_marco, uint32_t numero_pagina, uint32_t marco);
bool recv_num_marco(int fd, uint8_t* pid_marco, uint32_t* numero_pagina, uint32_t* marco);

// RECIBIR_VALOR_MEMORIA --> para MOV_IN
t_buffer* serializar_valor_memoria(void* valor, uint8_t tam_dato);
void send_valor_memoria(int fd, void* valor, uint8_t tam_dato);
bool recv_valor_memoria(int fd, void** valor, uint8_t* tam_dato);

// ESCRITURA_OK --> para MOV_OUT
void send_escritura_ok(int fd, uint8_t escritura_ok);
bool recv_escritura_ok(int fd, uint8_t* escritura_ok);

// FIN_PROCESO
void send_fin_proceso(int fd, uint8_t pid_fin);
bool recv_fin_proceso(int fd, uint8_t* pid_fin);

// IOs
// Nombre de interfaz
void send_interfaz(int fd, char* nombre_interfaz, uint32_t length);
bool recv_interfaz(int fd, char* nombre_interfaz);

// Finalizar IO
t_buffer* serializar_fin_io(char* nombre, uint32_t length);
void send_fin_io(int fd, char* nombre, uint32_t length);
bool recv_fin_io(int fd, char* nombre);

// IO_GEN_SLEEP
t_buffer* serializar_io_gen_sleep(t_pcb* pcb_io, uint32_t unidades_trabajo, char* nombre, uint32_t length);
void send_io_gen_sleep(int fd, t_pcb* pcb_io, uint32_t unidades_trabajo, char* nombre, uint32_t length);
bool recv_io_gen_sleep(int fd, t_pcb* pcb_io, uint32_t* unidades_trabajo, char* nombre);

// IO_STDIN_READ y IO_STDOUT_WRITE
t_buffer* serializar_io_stdin_stdout(t_pcb* pcb_io, uint32_t direccion_fisica, uint32_t tamanio_maximo, char* nombre, uint32_t length);
void send_io_stdin_read(int fd, t_pcb* pcb_io, uint32_t direccion_fisica, uint32_t tamanio_maximo, char* nombre, uint32_t length);
void send_io_stdout_write(int fd, t_pcb* pcb_io, uint32_t direccion_fisica, uint32_t tamanio_maximo, char* nombre, uint32_t length);
bool recv_io_stdin_stdout(int fd, t_pcb* pcb_io, uint32_t* direccion_fisica, uint32_t* tamanio_maximo, char* nombre);

#endif