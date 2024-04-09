#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdlib.h>

#include <commons/log.h>

t_log* kernel_logger;
t_log* kernel_debug;

void init_kernel_logs();

#endif