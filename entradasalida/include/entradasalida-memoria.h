#ifndef IO_MEMORIA_H_
#define IO_MEMORIA_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "configs.h"
#include "logs.h"

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"

extern t_pcb* pcb_stdin;
extern t_pcb* pcb_stdout;
extern char* nombre_stdin;
extern char* nombre_stdout;
extern char* nombre_stdin_recibido;
extern char* nombre_stdout_recibido;
extern char* string;
extern pthread_mutex_t mutexIO;

void conexion_entradasalida_memoria();

#endif