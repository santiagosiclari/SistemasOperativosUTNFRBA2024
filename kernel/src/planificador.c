#include "../include/planificador.h"

pthread_mutex_t colaNewMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaReadyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaBlockedMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaExecMutex = PTHREAD_MUTEX_INITIALIZER;

void planificacionFIFO() {
    while (!queue_is_empty(colaReady) || !queue_is_empty(colaNew) || !queue_is_empty(colaExec)) {
        while (queue_size(colaNew) > 0 && queue_size(colaReady) <= GRADO_MULTIPROGRAMACION) {
            pthread_mutex_lock(&colaNewMutex);
            t_pcb* pcb_nuevo = queue_pop(colaNew);
            pthread_mutex_unlock(&colaNewMutex);
            pcb_nuevo->estado = 'R';
            pthread_mutex_lock(&colaReadyMutex);
            queue_push(colaReady, pcb_nuevo);
            pthread_mutex_unlock(&colaReadyMutex);
            log_info(kernel_logger, "Se paso el proceso %d de New a Ready", pcb_nuevo->pid);
        }

        pthread_mutex_lock(&colaExecMutex);
        if (queue_size(colaReady) > 0 && queue_size(colaExec) == 0) {
            pthread_mutex_lock(&colaReadyMutex);
            t_pcb* pcb = queue_pop(colaReady);
            pthread_mutex_unlock(&colaReadyMutex);

            pcb->estado = 'E';
            queue_push(colaExec, pcb);

            // Manda PID del proceso a ejecutar
            send_pid(fd_cpu_dispatch, pcb->pid);
            log_info(kernel_logger, "Se paso el proceso %d de Ready a Exec", pcb->pid);
        } else {
            // Si no hay procesos listos --> espera un momento antes de verificar nuevamente
            pthread_mutex_unlock(&colaExecMutex);
            usleep(100);
        }
    }

    queue_clean_and_destroy_elements(colaNew, free);
    queue_clean_and_destroy_elements(colaReady, free);
    queue_clean_and_destroy_elements(colaExec, free);
    queue_clean_and_destroy_elements(colaBlocked, free);
}

void planificacionRR() {

}