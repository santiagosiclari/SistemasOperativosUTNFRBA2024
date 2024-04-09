#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdlib.h>

#include <commons/log.h>

t_log* entradasalida_logger;
t_log* entradasalida_debug;

void init_entradasalida_logs();

#endif