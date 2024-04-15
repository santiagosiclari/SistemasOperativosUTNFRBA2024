#ifndef CPU_KERNEL_D_H_
#define CPU_KERNEL_D_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "configs.h"
#include "logs.h"

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"

void conexion_cpu_kernel_dispatch();

#endif