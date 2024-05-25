#ifndef IO_KERNEL_H_
#define IO_KERNEL_H_

#include <stdlib.h>
#include <stdio.h>

#include "configs.h"
#include "logs.h"

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/pcb.h"

void conexion_entradasalida_kernel();

extern char* nombre;

#endif