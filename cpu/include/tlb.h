#ifndef TLB_H_
#define TLB_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>
#include "configs.h"

typedef struct {
    uint8_t pid;
    uint32_t pagina;
    uint32_t marco;
    uint32_t last_used; // Para LRU
} t_tlb;

extern t_list* lista_tlb;

t_tlb* buscar_en_tlb(uint8_t pid, uint32_t pagina);
void agregar_a_tlb(uint8_t pid, uint32_t pagina, uint32_t marco);
t_tlb* list_remove_least_recently_used();

#endif