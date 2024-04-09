#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdlib.h>

#include <commons/log.h>

t_log* memoria_logger;
t_log* memoria_debug;

void init_memoria_logs();

#endif