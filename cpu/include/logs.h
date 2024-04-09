#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdlib.h>

#include <commons/log.h>

t_log* cpu_logger;
t_log* cpu_debug;

void init_cpu_logs();

#endif