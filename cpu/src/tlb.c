#include "../include/tlb.h"

pthread_mutex_t tlb_mutex = PTHREAD_MUTEX_INITIALIZER;

t_tlb* buscar_en_tlb(uint8_t pid, uint32_t pagina) {
    pthread_mutex_lock(&tlb_mutex);
    t_tlb* tlb_entry_encontrada = NULL;
    for (int i = 0; i < list_size(lista_tlb); i++) {
        t_tlb* entrada = list_get(lista_tlb, i);
        if (entrada->pid == pid && entrada->pagina == pagina) {
            entrada->last_used = time(NULL); // Actualizar tiempo de acceso
            tlb_entry_encontrada = entrada;
            break;
        }
    }
    pthread_mutex_unlock(&tlb_mutex);
    return tlb_entry_encontrada;
}

void agregar_a_tlb(uint8_t pid, uint32_t pagina, uint32_t marco) {
    pthread_mutex_lock(&tlb_mutex);
    t_tlb* nueva_entrada = malloc(sizeof(t_tlb));
    nueva_entrada->pid = pid;
    nueva_entrada->pagina = pagina;
    nueva_entrada->marco = marco;
    nueva_entrada->last_used = time(NULL); // Registrar tiempo de acceso

    if (list_size(lista_tlb) >= CANTIDAD_ENTRADAS_TLB) {
        // Si la TLB está llena, eliminar la entrada según el algoritmo (FIFO o LRU)
        if (strcmp(ALGORITMO_TLB, "FIFO") == 0) {
            // Reemplazar FIFO: Eliminar la primera entrada que se agregó
            t_tlb* entrada_a_eliminar = list_remove(lista_tlb, 0);
            free(entrada_a_eliminar);
        } else if (strcmp(ALGORITMO_TLB, "LRU") == 0) {
            // Reemplazar LRU: Encontrar y eliminar la entrada menos recientemente utilizada
            t_tlb* menos_reciente = list_remove_least_recently_used();
            free(menos_reciente);
        }
    }

    list_add(lista_tlb, nueva_entrada);
    pthread_mutex_unlock(&tlb_mutex);
}

t_tlb* list_remove_least_recently_used() {
    // Si la TLB está vacía, retorna NULL
    if (list_is_empty(lista_tlb)) {
        return NULL;
    }

    t_tlb* menos_reciente = list_get(lista_tlb, 0); // Suponemos que el primer elemento es el menos reciente
    int index_menos_reciente = 0;

    // Buscamos la entrada menos reciente
    for (int i = 1; i < list_size(lista_tlb); i++) {
        t_tlb* entrada = list_get(lista_tlb, i);
        // Si la entrada actual fue usada antes que la considerada menos reciente
        if (entrada->last_used < menos_reciente->last_used) {
            menos_reciente = entrada;
            index_menos_reciente = i;
        }
    }

    // Eliminamos y retornamos la entrada menos reciente
    return list_remove(lista_tlb, index_menos_reciente);
}