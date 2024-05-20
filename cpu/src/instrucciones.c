#include "../include/instrucciones.h"

// size_t tamanioRegistro(registrosCPU registro){
//     if(registro >= AX && registro <= DX){
//         return sizeof(uint8_t);
//     } else{
//         return sizeof(uint32_t);
//     }
// }

// void* obtenerRegistro(registrosCPU registro){
//     void* lista_de_registros[11] = {
//         &miCPU.AX, &miCPU.BX, &miCPU.CX, &miCPU.DX,
//         &miCPU.EAX, &miCPU.EBX, &miCPU.ECX, &miCPU.EDX,
//         &miCPU.SI, &miCPU.DI, &miCPU.PC
//     };
//     if(registro >= 0 && registro < 11){
//         return (lista_de_registros[registro]);
//     } else{
//         return NULL;
//     }
// }

// Execute
// SET (Registro, Valor): Asigna al registro el valor pasado como parámetro.
// t_pcb* cpu_set(t_pcb* pcb, char** instruccion_separada) {
// }

// SUM (Registro Destino, Registro Origen): Suma al Registro Destino el Registro Origen y deja el resultado en el Registro Destino.
// t_pcb* cpu_sum(t_pcb* pcb, char** instruccion_separada) {
//     return pcb;
// }

// SUB (Registro Destino, Registro Origen): Resta al Registro Destino el Registro Origen y deja el resultado en el Registro Destino.
// t_pcb* cpu_sub(t_pcb* pcb, char** instruccion_separada) {
//     return pcb;
// }

// JNZ (Registro, Instrucción): Si el valor del registro es distinto de cero, actualiza el program counter al número de instrucción pasada por parámetro.
// t_pcb* cpu_jnz(t_pcb* pcb, char** instruccion_separada) {
//     return pcb;
// }

// IO_GEN_SLEEP (Interfaz, Unidades de trabajo): Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O a que realice un sleep por una cantidad de unidades de trabajo.
// t_pcb* cpu_io_gen_sleep(t_pcb* pcb, char** instruccion_separada) {
//     return pcb;
// }

void funcion_set(t_dictionary* dictionary_registros, char* registro, int valor) {
    if (strlen(registro) == 3 || !strcmp(registro, "SI") || !strcmp(registro, "DI")) {
        uint32_t *r_destino = dictionary_get(dictionary_registros, registro);
        *r_destino = valor;
    } else if (strlen(registro) == 2) {
        uint8_t *r_destino = dictionary_get(dictionary_registros, registro);
        *r_destino = valor;
    }

    pcb_a_ejecutar->pc++;
}

void funcion_sum(t_dictionary* dictionary_registros, char* registro_destino, char* registro_origen) {
    if (strlen(registro_destino) == 3 || !strcmp(registro_destino, "SI") || !strcmp(registro_destino, "DI")) {
        uint32_t *r_destino = dictionary_get(dictionary_registros, registro_destino);
        if (strlen(registro_origen) == 3 || !strcmp(registro_origen, "SI") || !strcmp(registro_origen, "DI")) {
            uint32_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino += *r_origen;
        } else if (strlen(registro_origen) == 2) {
            uint8_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino += *r_origen;
        }
    }
    else if (strlen(registro_destino) == 2) {
        uint8_t *r_destino = dictionary_get(dictionary_registros, registro_destino);
        if (strlen(registro_origen) == 3 || !strcmp(registro_origen, "SI") || !strcmp(registro_origen, "DI")) {
            uint32_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino += *r_origen;
        } else if (strlen(registro_destino) == 2) {
            uint8_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino += *r_origen;
        }
    }

    pcb_a_ejecutar->pc++;
}

void funcion_sub(t_dictionary* dictionary_registros, char* registro_destino, char* registro_origen) {
    if (strlen(registro_destino) == 3 || !strcmp(registro_destino, "SI") || !strcmp(registro_destino, "DI")) {
        uint32_t *r_destino = dictionary_get(dictionary_registros, registro_destino);
        if (strlen(registro_origen) == 3 || !strcmp(registro_origen, "SI") || !strcmp(registro_origen, "DI")) {
            uint32_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino -= *r_origen;
        } else if (strlen(registro_origen) == 2) {
            uint8_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino -= *r_origen;
        }
    } else if (strlen(registro_destino) == 2) {
        uint8_t *r_destino = dictionary_get(dictionary_registros, registro_destino);
        if (strlen(registro_origen) == 3 || !strcmp(registro_origen, "SI") || !strcmp(registro_origen, "DI")) {
            uint32_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino -= *r_origen;
        } else if (strlen(registro_destino) == 2) {
            uint8_t *r_origen = dictionary_get(dictionary_registros, registro_origen);
            *r_destino -= *r_origen;
        }
    }

    pcb_a_ejecutar->pc++;
}

void funcion_jnz(t_dictionary* dictionary_registros, char* registro, uint32_t valor_pc) {
    if (strlen(registro) == 3 || !strcmp(registro, "SI") || !strcmp(registro, "DI")) {
        uint32_t *r_registro = dictionary_get(dictionary_registros, registro);
        if(r_registro != 0) {
            pcb_a_ejecutar->pc = valor_pc;
        }
    } else if (strlen(registro) == 2) {
        uint8_t *r_registro = dictionary_get(dictionary_registros, registro);
        if(r_registro != 0) { 
            pcb_a_ejecutar->pc = valor_pc;
        }
    }
}

void funcion_io_gen_sleep(char* interfaz, uint32_t unidades_trabajo) {
    pcb_a_ejecutar->flag_io = 1; // Cuando envio el Contexto de ejecucion al Kernel, sabe que el proceso fue interrumpido por una interfaz IO
    send_pcb(fd_kernel_dispatch, pcb_a_ejecutar); // Envia el contexto de ejecucion
    // Enviar el pcb como contexto de ejecucion directamente en la funcion de send_io_gen_sleep
    send_io_gen_sleep(fd_kernel_dispatch, unidades_trabajo, interfaz, strlen(interfaz) + 1); // Envia el nombre de la interfaz
    pcb_a_ejecutar->pc++;
}

void funcion_exit() {
    log_info(cpu_logger, "El proceso finalizo");
    send_pid_a_borrar(fd_kernel_dispatch, pcb_a_ejecutar->pid);
    free(pcb_a_ejecutar->registros);
    free(pcb_a_ejecutar);
    pcb_a_ejecutar = NULL;
}

// void set(registrosCPU registroDestino, int valor){ //No muestra errores. anda?
//     void *registro_destino = obtenerRegistro(registroDestino);

//      if(registro_destino == NULL){
//         printf("Erorr al recibir el registro\n");
//         return;
//     }

//     size_t tam_destino = tamanioRegistro(registroDestino);

//     if(tam_destino == sizeof(uint32_t)){
//         *(uint32_t *)registro_destino = valor;
//     } else if(tam_destino == sizeof(uint8_t)){
//         *(uint8_t *)registro_destino = valor;
//     } else{
//         printf("Error al leer el tamanio del registro\n");
//     }
// }

// void sub(registrosCPU registroDestino, registrosCPU registroOrigen){ //No muestra errores. anda?
//     void *registro_destino = obtenerRegistro(registroDestino);
//     void *registro_origen = obtenerRegistro(registroOrigen);

//     if(registro_destino == NULL || registro_origen == NULL){
//         printf("Error al recibir los registros\n");
//         return;
//     }

//     size_t tam_destino = tamanioRegistro(registroDestino);
//     size_t tam_origen = tamanioRegistro(registroOrigen);

//     switch(tam_destino){
//         case sizeof(uint8_t):
//             switch (tam_origen){
//                 case sizeof(uint8_t):
//                     *(uint8_t *)registro_destino -= *(uint8_t *)registro_origen;
//                     break;
//                 case sizeof(uint32_t):
//                     *(uint8_t *)registro_destino -= *(uint32_t *)registro_origen;
//                     break;
//             }
//             break;
//         case sizeof(uint32_t):
//             switch (tam_origen){
//                 case sizeof(uint8_t ):
//                     *(uint32_t *)registro_destino -= *(uint8_t *)registro_origen;
//                     break;
//                 case sizeof(uint32_t):
//                     *(uint32_t *)registro_destino -= *(uint32_t *)registro_origen;
//                     break;
//             }
//             break;
//     }
// }

// void sum(registrosCPU registroDestino, registrosCPU registroOrigen){ //No muestra errores. anda?
//     void *registro_destino = obtenerRegistro(registroDestino);
//     void *registro_origen = obtenerRegistro(registroOrigen);

//     if(registro_destino == NULL || registro_origen == NULL){
//         printf("Erorr al recibir los registros\n");
//         return;
//     }

//     size_t tam_destino = tamanioRegistro(registroDestino);
//     size_t tam_origen = tamanioRegistro(registroOrigen);

//     switch(tam_destino){
//         case sizeof(uint8_t):
//             switch (tam_origen){
//                 case sizeof(uint8_t):
//                     *(uint8_t *)registro_destino += *(uint8_t *)registro_origen;
//                     break;
//                 case sizeof(uint32_t):
//                     *(uint8_t *)registro_destino += *(uint32_t *)registro_origen;
//                     break;
//             }
//             break;
//         case sizeof(uint32_t):
//             switch (tam_origen){
//                 case sizeof(uint8_t ):
//                     *(uint32_t *)registro_destino += *(uint8_t *)registro_origen;
//                     break;
//                 case sizeof(uint32_t):
//                     *(uint32_t *)registro_destino += *(uint32_t *)registro_origen;
//                     break;
//             }
//             break;
//     }
// }

// uint32_t jnz(registrosCPU registro, char* instruccion){ //uint32_t para despues igualarlo a pcb->pc = jnz..
//     void *registro_destino = obtenerRegistro(registro);

//      if(registro_destino == NULL){
//         printf("Error al recibir el registro\n");
//         return -1;
//     }

//     size_t tam_registro = tamanioRegistro(registro);

//     uint32_t num_instruccion = atoi(instruccion);

//         // Evaluar el valor del registro
//     switch (tam_registro) {
//        case sizeof(uint8_t):
//             if (*(uint8_t*)registro_destino != 0) {
//                 return num_instruccion;
//             } else {
//                 printf("Registro es igual a 0\n");
//             }
//             break;
//         case sizeof(uint32_t):
//             if (*(uint32_t*)registro_destino != 0) {
//                 return num_instruccion;
//             } else{
//                 printf("Registro es igual a 0\n");
//             }
//             break;
//         default:
//             printf("Registro no válido\n");
//             break;
//     }
//     return 0;
// }
// 
// void io_gen_sleep(char* interfaz, int unidades_trabajo){
//     int microseg_unidades_trabajo = unidades_trabajo * 1000;//para el usleep, milisegundos
//     //faltaria hacer el envio del kernel a la interfaz
//     usleep(microseg_unidades_trabajo);
// }