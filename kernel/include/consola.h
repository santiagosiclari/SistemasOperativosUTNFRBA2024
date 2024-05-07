#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/string.h>

#include "logs.h"
#include "funciones-pcb.h"
#include <readline/readline.h>

#include <../../utils/include/file-descriptors.h>
#include <../../utils/include/protocolo.h>

void iniciar_consola(t_log* logger);

#endif