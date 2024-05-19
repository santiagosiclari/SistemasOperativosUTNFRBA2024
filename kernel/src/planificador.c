#include "../include/planificador.h"

pthread_mutex_t colaNewMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaReadyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaBlockedMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaExecMutex = PTHREAD_MUTEX_INITIALIZER;

void planificacionFIFO() {
    if(queue_size(colaNew) > 0) {
        if(queue_size(colaReady) <= GRADO_MULTIPROGRAMACION) {
          pthread_mutex_lock(&colaNewMutex);
          t_pcb* pcb_nuevo = queue_pop(colaNew);
          pcb_nuevo->estado = 'R';
          queue_push(colaReady, pcb_nuevo);
          log_info(kernel_logger, "Se paso el proceso %d de New a Ready", pcb_nuevo->pid);
          pthread_mutex_unlock(&colaNewMutex);
          while (queue_size(colaExec) == 0 || queue_size(colaReady) != 0) {
            pthread_mutex_lock(&colaExecMutex);
            t_pcb* pcb = queue_pop(colaReady);
            pcb->estado = 'E';
            queue_push(colaExec, pcb);

            // Manda PID del proceso a ejecutar
            send_pid(fd_cpu_dispatch, pcb->pid);
            log_info(kernel_logger, "Se paso el proceso %d de Ready a Exec", pcb->pid);
            pthread_mutex_unlock(&colaExecMutex);

            if (queue_size(colaBlocked) > 0 && queue_size(colaReady) <= GRADO_MULTIPROGRAMACION) {
              t_pcb* pcb_block = queue_pop(colaBlocked);
              queue_push(colaReady, pcb_block);
            }

            usleep(1000);
          }
        }
    }

    queue_clean_and_destroy_elements(colaNew, free);
    queue_clean_and_destroy_elements(colaReady, free);
    queue_clean_and_destroy_elements(colaExec, free);
    queue_clean_and_destroy_elements(colaBlocked, free);
}

void planificacionRR() {

}