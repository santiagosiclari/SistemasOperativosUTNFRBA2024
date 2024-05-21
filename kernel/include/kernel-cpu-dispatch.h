#ifndef KERNEL_CPU_DISPATCH_H_
#define KERNEL_CPU_DISPATCH_H_

#include <stdlib.h>
#include <stdio.h>

#include "configs.h"
#include "logs.h"
#include <pthread.h>
#include <commons/collections/queue.h>

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"

extern t_queue* colaExec;
extern t_queue* colaReady;
extern t_queue* colaBlocked;

extern pthread_mutex_t colaExecMutex;
extern pthread_mutex_t colaReadyMutex;
extern pthread_mutex_t colaBlockedMutex;

void conexion_kernel_cpu_dispatch();

#endif