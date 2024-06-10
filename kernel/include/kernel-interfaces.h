#ifndef KERNEL_INTERFACES_H_
#define KERNEL_INTERFACES_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/collections/queue.h>

#include "logs.h"
#include "configs.h"

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/io.h"

extern t_queue* colaBlocked;
extern t_queue* colaReady;
extern t_queue* colaAux;

extern pthread_mutex_t colaBlockedMutex;
extern pthread_mutex_t colaReadyMutex;
extern pthread_mutex_t colaAuxMutex;

extern char* nombre_interfaz;

extern t_list* listaInterfaces;

void conexion_kernel_interfaces(void* arg);

#endif