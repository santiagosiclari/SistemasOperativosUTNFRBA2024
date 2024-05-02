#ifndef CPU_MAIN_H_
#define CPU_MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "configs.h"
#include "logs.h"
#include "cpu-kernel-dispatch.h"
#include "cpu-kernel-interrupt.h"
#include "terminar-cpu.h"

#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/file-descriptors.h"

int fd_kernel_dispatch;
int fd_kernel_interrupt;
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_memoria;

t_log* cpu_logger;
t_log* cpu_debug;

t_config* cpu_config;

// Declaracion archivos de configuracion
char* IP_MEMORIA;
char* PUERTO_MEMORIA; //char para func
char* PUERTO_ESCUCHA_DISPATCH;//char para func
char* PUERTO_ESCUCHA_INTERRUPT;//char para func
int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

typedef enum {
    NEW,
    READY,
    BLOCKED,
    EXEC,
    EXIT
} contexto_ejecucion;

typedef struct {
    uint8_t AX;
    uint8_t BX;
    uint8_t CX;
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t SI;
    uint32_t DI;
} t_registros;

typedef struct {
    uint8_t pid;
    uint32_t pc;
    uint8_t quantum;
    contexto_ejecucion conexto;
    t_registros registros;
} t_pcb;

#endif