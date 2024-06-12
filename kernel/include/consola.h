#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/string.h>
#include <commons/collections/queue.h>

#include "logs.h"
#include "funciones-pcb.h"
#include "planificador.h"
#include "recursos.h"
#include <readline/readline.h>

#include <../../utils/include/file-descriptors.h>
#include <../../utils/include/protocolo.h>

extern t_queue* colaNew;
extern t_queue* colaReady;
extern t_queue* colaBlocked;
extern t_queue* colaExec;

extern t_list* recursos_de_procesos;
extern t_list* recursos;

extern pthread_mutex_t colaNewMutex;

void iniciar_consola(t_log* logger);

#endif