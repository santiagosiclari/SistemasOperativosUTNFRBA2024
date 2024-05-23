#ifndef KERNEL_IO_H_
#define KERNEL_IO_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "configs.h"
#include "logs.h"

#include <commons/collections/queue.h>

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/io.h"

extern t_queue* colaBlocked;
extern t_queue* colaReady;

extern pthread_mutex_t colaBlockedMutex;
extern pthread_mutex_t colaReadyMutex;

extern char* nombre_interfaz;

extern t_list* listaInterfaces;

void conexion_kernel_entradasalida();

#endif