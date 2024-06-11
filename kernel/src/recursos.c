#include "../include/recursos.h"

t_list* recursos_de_procesos;
t_list* recursos;

void iniciar_recursos() {
    recursos = list_create();
    recursos_de_procesos = list_create();

    for(int i = 0; RECURSOS[i] != NULL; i++) {
        t_recurso* recurso = malloc(sizeof(t_recurso));
        recurso->nombre = strdup(RECURSOS[i]);
        recurso->instancias = atoi(INSTANCIAS_RECURSOS[i]);
        t_queue* colaBlockedRecurso = queue_create();
        recurso->blocked = colaBlockedRecurso;
        list_add(recursos, recurso);
    }
}

void liberar_recursos(uint8_t pid) {
    t_list* rec_tom = list_get(recursos_de_procesos, pid);
	for(int i = 0; i < list_size(rec_tom); i++) {
		t_recurso* r = list_get(rec_tom, i);
        r->instancias++;
		if(!queue_is_empty(r->blocked)) {
            t_pcb* pcb_desbloqueado = queue_pop(r->blocked);
            pthread_mutex_lock(&colaReadyMutex);
            queue_push(colaReady, pcb_desbloqueado);
            pthread_mutex_unlock(&colaReadyMutex);
	    }
    }
}