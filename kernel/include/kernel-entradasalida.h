#ifndef KERNEL_IO_H_
#define KERNEL_IO_H_

#include <stdlib.h>
#include <stdio.h>

#include "configs.h"
#include "logs.h"

#include <commons/collections/queue.h>

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"

extern t_queue* colaBlocked;
extern t_queue* colaReady;

void conexion_kernel_entradasalida();

#endif