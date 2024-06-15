#ifndef KERNEL_CPU_DISPATCH_H_
#define KERNEL_CPU_DISPATCH_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "configs.h"
#include "logs.h"
#include "recursos.h"
#include <pthread.h>
#include <commons/temporal.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/io.h"
#include "../../utils/include/pcb.h"

extern t_queue* colaNew;
extern t_queue* colaExec;
extern t_queue* colaReady;
extern t_queue* colaBlocked;
extern t_queue* colaAux;

extern pthread_t quantum_thread;

extern t_temporal* tiempo_vrr;
extern int control_planificacion;

extern t_list* listaInterfaces;

extern pthread_mutex_t colaExecMutex;
extern pthread_mutex_t colaReadyMutex;
extern pthread_mutex_t colaBlockedMutex;

extern t_list* recursos_de_procesos;
extern t_list* recursos;

void conexion_kernel_cpu_dispatch();

#endif