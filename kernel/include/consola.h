#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>

#include "logs.h"
#include <readline/readline.h>

extern t_log* logger;

void iniciar_consola(t_log* logger);

#endif