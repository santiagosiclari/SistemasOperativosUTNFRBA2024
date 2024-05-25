#include "../include/planificador.h"

pthread_mutex_t colaNewMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaReadyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaBlockedMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t colaExecMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t quantum_thread;

int control_planificacion;

void destruir_pcb(void* ptr_pcb) {
    t_pcb* pcb = (t_pcb*)ptr_pcb;
    // Liberar recursos del PCB
    if (pcb && pcb->registros) {
        free(pcb->registros);
    }
    if (pcb) {
        free(pcb);
    }
}

void* quantum(void* arg) {
    t_pcb* pcb = (t_pcb*)arg;
    
    usleep(pcb->quantum);
    send_interrupcion(fd_cpu_interrupt, pcb->pid);

    return NULL;
}

void controlar_quantum(t_pcb* pcb) {
    pthread_create(&quantum_thread, NULL, (void *)quantum, (void *)pcb);
    pthread_detach(quantum_thread);
}

void planificacionFIFO() {
    // El control_planificacion se cambia a 0 una vez que se elimina el proceso y todas las queues estan vacias
    // Esto es por si solamente hay un proceso ejecutando y se interrumpe para que no haya problemas
    control_planificacion = 1;
    while (control_planificacion) {
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
    }

    queue_clean_and_destroy_elements(colaNew, destruir_pcb);
    queue_clean_and_destroy_elements(colaReady, destruir_pcb);
    queue_clean_and_destroy_elements(colaExec, destruir_pcb);
    queue_clean_and_destroy_elements(colaBlocked, destruir_pcb);
}

void planificacionRR() {
    // El control_planificacion se cambia a 0 una vez que se elimina el proceso y todas las queues estan vacias
    // Esto es por si solamente hay un proceso ejecutando y se interrumpe para que no haya problemas
    control_planificacion = 1;
    while (control_planificacion) {
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
            if (queue_size(colaReady) > 0 || queue_size(colaBlocked) > 0 || queue_size(colaExec) != 0) {
                controlar_quantum(pcb);
            }
        }
    }

    queue_clean_and_destroy_elements(colaNew, destruir_pcb);
    queue_clean_and_destroy_elements(colaReady, destruir_pcb);
    queue_clean_and_destroy_elements(colaExec, destruir_pcb);
    queue_clean_and_destroy_elements(colaBlocked, destruir_pcb);
}