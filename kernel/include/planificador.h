#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "logs.h"
#include "configs.h"
#include "consola.h"
#include "semaphore.h"

#include <pthread.h>
#include <semaphore.h>

#include <commons/collections/queue.h>

#include "../../utils/include/pcb.h"

extern t_queue* colaNew;
extern t_queue* colaReady;
extern t_queue* colaBlocked;
extern t_queue* colaExec;

void planificacionFIFO();
void planificacionRR();

extern pthread_mutex_t colaNewMutex;
extern pthread_mutex_t colaReadyMutex;
extern pthread_mutex_t colaBlockedMutex;
extern pthread_mutex_t colaExecMutex;

#endif