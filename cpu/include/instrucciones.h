#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <stdio.h>
#include <stdlib.h>

#include "../../utils/include/protocolo.h"

t_pcb* cpu_set(t_pcb* pcb, char** instruccion_separada);
t_pcb* cpu_sum(t_pcb* pcb, char** instruccion_separada);
t_pcb* cpu_sub(t_pcb* pcb, char** instruccion_separada);
t_pcb* cpu_jnz(t_pcb* pcb, char** instruccion_separada);
t_pcb* cpu_io_gen_sleep(t_pcb* pcb, char** instruccion_separada);

#endif