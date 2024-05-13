#ifndef MEMORIA_CPU_H_
#define MEMORIA_CPU_H_

#include <stdlib.h>
#include <stdio.h>

#include "configs.h"
#include "logs.h"
#include "archivos.h"

#include <commons/collections/list.h>

#include "../include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"

extern t_list* instrucciones_por_proceso;

void conexion_memoria_cpu();

#endif