#include "../include/planificador.h"

pthread_mutex_t colaNewMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaReadyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaBlockedMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaExecMutex = PTHREAD_MUTEX_INITIALIZER;

void* quantum(void* arg){
    t_pcb* pcb = (t_pcb*)arg;
    usleep(pcb->quantum);
    send_interrupcion(fd_cpu_interrupt, pcb->pid);
    return NULL;
}

void controlar_quantum(t_pcb* pcb) {
    pthread_t quantum_thread;
    pthread_create(&quantum_thread, NULL, (void *)quantum, (void *)pcb);
    pthread_join(quantum_thread, NULL);
}

void planificacionFIFO() {
    while (!queue_is_empty(colaReady) || !queue_is_empty(colaNew) || !queue_is_empty(colaExec) || !queue_is_empty(colaBlocked)) {
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

        if (queue_size(colaReady) > 0 && queue_size(colaExec) == 0) {
            pthread_mutex_lock(&colaExecMutex);
            pthread_mutex_lock(&colaReadyMutex);
            t_pcb* pcb = queue_pop(colaReady);
            pthread_mutex_unlock(&colaReadyMutex);
            pcb->estado = 'E';
            queue_push(colaExec, pcb);
            pthread_mutex_unlock(&colaExecMutex);

            // Manda PID del proceso a ejecutar
            send_pid(fd_cpu_dispatch, pcb->pid);
            log_info(kernel_logger, "Se paso el proceso %d de Ready a Exec", pcb->pid);
        }
        
        usleep(100);
    }
}

void planificacionRR() {
    while (!queue_is_empty(colaReady) || !queue_is_empty(colaNew) || !queue_is_empty(colaExec) || !queue_is_empty(colaBlocked)) {
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

        if (queue_size(colaReady) > 0 && queue_size(colaExec) == 0) {
            pthread_mutex_lock(&colaExecMutex);
            pthread_mutex_lock(&colaReadyMutex);
            t_pcb* pcb = queue_pop(colaReady);
            pthread_mutex_unlock(&colaReadyMutex);
            pcb->estado = 'E';
            queue_push(colaExec, pcb);
            pthread_mutex_unlock(&colaExecMutex);

            // Manda PID del proceso a ejecutar
            send_pid(fd_cpu_dispatch, pcb->pid);
            log_info(kernel_logger, "Se paso el proceso %d de Ready a Exec", pcb->pid);
            
            // Quantum
            controlar_quantum(pcb);
        }

        usleep(100);
    }
}