#ifndef CPU_KERNEL_DISPATCH_H_
#define CPU_KERNEL_DISPATCH_H_

#include <stdlib.h>
#include <stdio.h>

#include "configs.h"
#include "logs.h"
#include "../../utils/include/pcb.h"

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"

extern t_pcb* pcb;

void conexion_cpu_kernel_dispatch();

#endif