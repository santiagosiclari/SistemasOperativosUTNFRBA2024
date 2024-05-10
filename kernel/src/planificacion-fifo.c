// #include "../include/planificacion-fifo.h"

// void planificacionFIFO() {
//     int grado_multitarea = GRADO_MULTIPROGRAMACION;
//     int tamanioColaReady = queue_size(colaReady);
//     int tamanioColaExc = queue_size(colaExec);
//     int tamanioColaBlocked = queue_size(colaBlocked);

//     if(tamanioColaReady > 0)
//     {
//         if(tamanioColaExc < grado_multitarea)
//         {
//           int i = 0;
//           while (tamanioColaExc < grado_multitarea && (tamanioColaReady - i ) > 0)
//           {
//             //semaforo
//             t_pcb* pcb = queue_peek(colaReady);
//             pcb->estado = 'E';
//             queue_pop(colaReady);
//             //semaforo
//             //avisar cambio de estado
//             queue_push(colaExec, pcb);
//             log_info(kernel_logger,"Se pasaron algunos procesos de ready a Exec");
//             i++;
//             tamanioColaExc++;
//           }

//           //Enviar cola de ejecutados para que los procese la CPU por dispatch

//           if (tamanioColaBlocked > 0)
//           {
//             //En esta parte se deberian pasar los procesos que se encuentran bloqueados 
//             //al final de la cola de Ready por ser planificacion FIFO
//           }

//         }
//     }
// }