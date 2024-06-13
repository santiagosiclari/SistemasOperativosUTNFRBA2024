#ifndef IO_KERNEL_H_
#define IO_KERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>

#include "configs.h"
#include "logs.h"

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/pcb.h"

extern t_pcb* pcb_stdout;
extern t_pcb* pcb_stdin;
extern pthread_mutex_t mutexIO;

void conexion_entradasalida_kernel();

extern char* nombre;
extern char* nombre_stdin;
extern char* nombre_stdout;
extern char* nombre_stdin_recibido;
extern char* nombre_stdout_recibido;
extern char* string;

#endif