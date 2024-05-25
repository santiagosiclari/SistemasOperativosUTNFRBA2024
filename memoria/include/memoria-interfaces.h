#ifndef MEMORIA_INTERFACES_H_
#define MEMORIA_INTERFACES_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "logs.h"
#include "configs.h"

#include "../../utils/include/file-descriptors.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/io.h"

void conexion_memoria_interfaces(void* arg);

#endif