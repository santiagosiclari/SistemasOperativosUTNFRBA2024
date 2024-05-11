#include "../include/planificador.h"

void planificacionFIFO() {
    // Cola New ya creada en consola
    t_queue* colaReady = queue_create();
    t_queue* colaBlocked = queue_create();
    t_queue* colaExec = queue_create();

    int grado_multitarea = GRADO_MULTIPROGRAMACION;
    int tamanioColaReady = queue_size(colaReady);
    int tamanioColaExc = queue_size(colaExec);
    int tamanioColaBlocked = queue_size(colaBlocked);

    if(tamanioColaReady > 0)
    {
        if(tamanioColaExc < grado_multitarea)
        {
          int i = 0;
          while (tamanioColaExc < grado_multitarea && (tamanioColaReady - i) > 0)
          {
            //semaforo
            t_pcb* pcb = queue_peek(colaReady);
            pcb->estado = 'E';
            queue_pop(colaReady);

            // Manda PID del proceso a ejecutar
            send_pid(fd_cpu_dispatch, pcb->pid);

            //semaforo
            //avisar cambio de estado
            queue_push(colaExec,pcb);
            log_info(kernel_logger,"Se pasaron algunos procesos de ready a Exec");
            i++;
            tamanioColaExc++;
          }

          if (tamanioColaBlocked > 0) {
            //En esta parte se deberian pasar los procesos que se encuentran bloqueados 
            //al final de la cola de Ready por ser planificacion FIFO
          }
        }
    }
}