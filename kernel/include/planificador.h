#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "logs.h"
#include "configs.h"
#include "consola.h"
#include "semaphore.h"

#include <pthread.h>

#include <commons/collections/queue.h>

#include "../../utils/include/pcb.h"

extern t_queue* colaNew;
extern t_queue* colaReady;
extern t_queue* colaBlocked;
extern t_queue* colaExec;

void planificacionFIFO();
void planificacionRR();

#endif