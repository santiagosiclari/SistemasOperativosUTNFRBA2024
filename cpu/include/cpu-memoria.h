#ifndef CPU_MEMORIA_H_
#define CPU_MEMORIA_H_

#include <stdlib.h>
#include <stdio.h>

#include "configs.h"
#include "logs.h"
#include "string.h"

#include <commons/string.h>

#include "instrucciones.h"

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"

extern t_pcb* pcb;
extern uint8_t size_instrucciones;

void conexion_cpu_memoria();

#endif