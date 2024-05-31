#ifndef CPU_MEMORIA_H_
#define CPU_MEMORIA_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "configs.h"
#include "logs.h"
#include "string.h"

#include <commons/string.h>
#include <commons/collections/dictionary.h>

#include "instrucciones.h"

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"

extern t_pcb* pcb_a_ejecutar;
extern uint32_t tam_pagina;
extern t_list* lista_tlb;
extern pthread_mutex_t pcbEjecutarMutex;

void conexion_cpu_memoria();
void crear_diccionario(t_dictionary* dictionary_registros);

#endif