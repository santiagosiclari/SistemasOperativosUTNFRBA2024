#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <commons/collections/queue.h>
#include "logs.h"
#include "configs.h"
#include "../../utils/include/pcb.h"

//Colas de planificacion corto plazo
extern t_queue* colaReady;
extern t_queue* colaExec;
extern t_queue* colaBlocked;


#endif