#include "../include/protocolo.h"

// Funciones de send y recv del protocolo
// Handshake
void send_handshake(t_log* logger, int fd, int32_t handshake, const char *conexion) {
    size_t bytes;
    int32_t result;

    bytes = send(fd, &handshake, sizeof(int32_t), 0);
    bytes = recv(fd, &result, sizeof(int32_t), MSG_WAITALL);

    if (result == 0 && bytes != 0) {
        log_info(logger, "Handshake OK de %s\n", conexion);
    }
    else {
        log_error(logger, "Handshake ERROR de %s\n", conexion);
    }
}

bool recv_handshake(int fd, int32_t handshakeModulo) {
    size_t bytes;
    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    bytes = recv(fd, &handshake, sizeof(int32_t), MSG_WAITALL);
    if (handshake == handshakeModulo && bytes != 0) {
        bytes = send(fd, &resultOk, sizeof(int32_t), 0);
        return true;
    }
    else {
        bytes = send(fd, &resultError, sizeof(int32_t), 0);
        return false;
    }
}

// uint32_t
t_buffer* serializar_uint32(uint32_t entero) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t);

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint32_al_buffer(buffer, entero);

    return buffer;
}

t_buffer* serializar_uint8(uint8_t entero) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint8_t);

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint8_al_buffer(buffer, entero);

    return buffer;
}

// PID
void send_pid(int fd, uint8_t pid) {
    t_buffer *buffer = serializar_uint8(pid);
    t_paquete *a_enviar = crear_paquete(RECIBIR_PID, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_pid(int fd, uint8_t* pid) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    *pid = extraer_uint8_del_buffer(paquete->buffer);

    eliminar_paquete(paquete);

    return true;
}

// PID a borrar (EXIT)
void send_pid_a_borrar(int fd, uint8_t pid_a_borrar) {
    t_buffer *buffer = serializar_uint8(pid_a_borrar);
    t_paquete *a_enviar = crear_paquete(RECIBIR_PID_A_BORRAR, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_pid_a_borrar(int fd, uint8_t* pid_a_borrar) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    *pid_a_borrar = extraer_uint8_del_buffer(paquete->buffer);

    eliminar_paquete(paquete);

    return true;
}

// Interrupcion (por conexion interrupt) --> a CPU
void send_interrupcion(int fd, uint8_t pid_a_interrumpir) {
    t_buffer *buffer = serializar_uint8(pid_a_interrumpir);
    t_paquete *a_enviar = crear_paquete(RECIBIR_PID_A_INTERRUMPIR, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_interrupcion(int fd, uint8_t* pid_a_interrumpir) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    *pid_a_interrumpir = extraer_uint8_del_buffer(paquete->buffer);

    eliminar_paquete(paquete);

    return true;
}

// send y recv pc y pid (juntos) --> para fetch
t_buffer* serializar_pc_pid(uint32_t pc, uint8_t pid) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = 
        sizeof(uint32_t) +  // PC
        sizeof(uint8_t);    // PID

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint32_al_buffer(buffer, pc);
    cargar_uint8_al_buffer(buffer, pid);

    return buffer;
}

void send_pc_pid(int fd, uint32_t pc, uint8_t pid) {
    t_buffer *buffer = serializar_pc_pid(pc, pid);
    t_paquete *a_enviar = crear_paquete(RECIBIR_PC_PID, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_pc_pid(int fd, uint32_t* pc, uint8_t* pid) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    *pc = extraer_uint32_del_buffer(paquete->buffer);
    *pid = extraer_uint8_del_buffer(paquete->buffer);

    eliminar_paquete(paquete);

    return true;
}

// Size instrucciones
void send_size_instrucciones(int fd, uint8_t size) {
    t_buffer *buffer = serializar_uint8(size);
    t_paquete *a_enviar = crear_paquete(RECIBIR_SIZE_INSTRUCCIONES, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_size_instrucciones(int fd, uint8_t* size) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    *size = extraer_uint8_del_buffer(paquete->buffer);

    eliminar_paquete(paquete);

    return true;
}

// PCB --> tambien utilizado como contexto de ejecucion
// Serializar PCB
t_buffer* serializar_pcb(t_pcb* pcb) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size =
        sizeof(uint8_t) +    // pid
        sizeof(uint32_t) +   // pc
        sizeof(char) +       // estado
        sizeof(uint32_t) +   // quantum
        sizeof(uint8_t) +    // flag_int
        sizeof(uint8_t) * 4 + sizeof(uint32_t) * 6; // tamaño de registros

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint8_al_buffer(buffer, pcb->pid);
    cargar_uint32_al_buffer(buffer, pcb->pc);
    cargar_char_al_buffer(buffer, pcb->estado);
    cargar_uint32_al_buffer(buffer, pcb->quantum);
    cargar_uint8_al_buffer(buffer, pcb->flag_int);

    // Registros
    cargar_uint8_al_buffer(buffer, pcb->registros->AX);
    cargar_uint8_al_buffer(buffer, pcb->registros->BX);
    cargar_uint8_al_buffer(buffer, pcb->registros->CX);
    cargar_uint8_al_buffer(buffer, pcb->registros->DX);
    cargar_uint32_al_buffer(buffer, pcb->registros->EAX);
    cargar_uint32_al_buffer(buffer, pcb->registros->EBX);
    cargar_uint32_al_buffer(buffer, pcb->registros->ECX);
    cargar_uint32_al_buffer(buffer, pcb->registros->EDX);
    cargar_uint32_al_buffer(buffer, pcb->registros->SI);
    cargar_uint32_al_buffer(buffer, pcb->registros->DI);

    return buffer;
}

// Deserializar PCB
t_pcb* deserializar_pcb(t_buffer *buffer, t_pcb *pcb) {
    // Datos del pcb
    pcb->pid = extraer_uint8_del_buffer(buffer);
    pcb->pc = extraer_uint32_del_buffer(buffer);
    pcb->estado = extraer_char_del_buffer(buffer);
    pcb->quantum = extraer_uint32_del_buffer(buffer);
    pcb->flag_int = extraer_uint8_del_buffer(buffer);

    // Registros
    pcb->registros->AX = extraer_uint8_del_buffer(buffer);
    pcb->registros->BX = extraer_uint8_del_buffer(buffer);
    pcb->registros->CX = extraer_uint8_del_buffer(buffer);
    pcb->registros->DX = extraer_uint8_del_buffer(buffer);
    pcb->registros->EAX = extraer_uint32_del_buffer(buffer);
    pcb->registros->EBX = extraer_uint32_del_buffer(buffer);
    pcb->registros->ECX = extraer_uint32_del_buffer(buffer);
    pcb->registros->EDX = extraer_uint32_del_buffer(buffer);
    pcb->registros->SI = extraer_uint32_del_buffer(buffer);
    pcb->registros->DI = extraer_uint32_del_buffer(buffer);

    return pcb;
}

void send_pcb(int fd, t_pcb *pcb) {
    t_buffer *buffer = serializar_pcb(pcb);
    t_paquete *a_enviar = crear_paquete(RECIBIR_PCB, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_pcb(int fd, t_pcb* pcb) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    deserializar_pcb(paquete->buffer, pcb);

    eliminar_paquete(paquete);

    return true;
}

// Char* (String)
t_buffer* serializar_string(char* string, uint32_t length) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size =
        sizeof(uint32_t) + // longitud del string
        length;            // string

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_string_al_buffer(buffer, string);

    return buffer;
}

char* deserializar_string(t_buffer* buffer) {
    char* string = extraer_string_del_buffer(buffer);
    return string;
}

// INICIAR_PROCESO
// Serializacion path y pid
t_buffer* serializar_iniciar_proceso(uint8_t pid, char* path, uint32_t length) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size =
        sizeof(uint8_t) +  // pid
        sizeof(uint32_t) + // longitud del path
        length;            // path

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint8_al_buffer(buffer, pid);
    cargar_string_al_buffer(buffer, path);

    return buffer;
}

void send_iniciar_proceso(int fd, uint8_t pid, char* path, uint32_t length) {
    t_buffer *buffer = serializar_iniciar_proceso(pid, path, length);
    t_paquete *a_enviar = crear_paquete(INICIAR_PROCESO, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_iniciar_proceso(int fd, uint8_t* pid, char* path) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);
    if (bytes_recibidos != paquete->buffer->size) {
        free(paquete->buffer->stream);
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    *pid = extraer_uint8_del_buffer(paquete->buffer);

    char* received_path = deserializar_string(paquete->buffer);
    strcpy(path, received_path);
    free(received_path);

    eliminar_paquete(paquete);
    return true;
}

// Instruccion
void send_instruccion(int fd, char* instruccion, uint32_t length) {
    t_buffer *buffer = serializar_string(instruccion, length);
    t_paquete *a_enviar = crear_paquete(RECIBIR_INSTRUCCION, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_instruccion(int fd, char* instruccion) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    char* received_instruccion = deserializar_string(paquete->buffer);
    strcpy(instruccion, received_instruccion);
    free(received_instruccion);

    eliminar_paquete(paquete);

    return true;
}

// TAM_PAGINA
void send_tam_pagina(int fd, uint32_t tam_pagina) {
    t_buffer *buffer = serializar_uint32(tam_pagina);
    t_paquete *a_enviar = crear_paquete(RECIBIR_TAM_PAGINA, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_tam_pagina(int fd, uint32_t* tam_pagina) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    *tam_pagina = extraer_uint32_del_buffer(paquete->buffer);

    eliminar_paquete(paquete);

    return true;
}

// Resize
void send_tamanio(int fd, uint32_t tamanio) {
    t_buffer *buffer = serializar_uint32(tamanio);
    t_paquete *a_enviar = crear_paquete(RECIBIR_TAMANIO, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_tamanio(int fd, uint32_t* tamanio) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    *tamanio = extraer_uint32_del_buffer(paquete->buffer);

    eliminar_paquete(paquete);

    return true;
}

// Interfaces IO
// Envio de nombre de interfaz
void send_interfaz(int fd, char* nombre_interfaz, uint32_t length) {
    t_buffer *buffer = serializar_string(nombre_interfaz, length);
    t_paquete *a_enviar = crear_paquete(RECIBIR_NOMBRE_IO, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_interfaz(int fd, char* nombre_interfaz) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    char* nombre_interfaz_recibido = deserializar_string(paquete->buffer);
    strcpy(nombre_interfaz, nombre_interfaz_recibido);
    free(nombre_interfaz_recibido);

    eliminar_paquete(paquete);

    return true;
}

// Fin de IO
t_buffer* serializar_fin_io(char* nombre, uint32_t length) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size =
        sizeof(uint32_t) +    // longitud del nombre
        length;               // nombre


    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_string_al_buffer(buffer, nombre);

    return buffer;
}

void send_fin_io(int fd, char* nombre, uint32_t length) {
    t_buffer *buffer = serializar_fin_io(nombre, length);
    t_paquete *a_enviar = crear_paquete(FIN_IO, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_fin_io(int fd, char* nombre) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    char* received_nombre = deserializar_string(paquete->buffer);
    strcpy(nombre, received_nombre);
    free(received_nombre);

    eliminar_paquete(paquete);

    return true;
}

// IO_GEN_SLEEP
t_buffer* serializar_io_gen_sleep(t_pcb* pcb_io, uint32_t unidades_trabajo, char* nombre, uint32_t length) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size =
        sizeof(uint8_t) +     // pid
        sizeof(uint32_t) +    // pc
        sizeof(char) +        // estado
        sizeof(uint32_t) +    // quantum
        sizeof(uint8_t) +     // flag_int
        sizeof(uint8_t) * 4 + sizeof(uint32_t) * 6 + // tamaño de registros
        sizeof(uint32_t) +    // unidades_trabajo
        sizeof(uint32_t) +    // longitud del nombre
        length;               // nombre

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint8_al_buffer(buffer, pcb_io->pid);
    cargar_uint32_al_buffer(buffer, pcb_io->pc);
    cargar_char_al_buffer(buffer, pcb_io->estado);
    cargar_uint32_al_buffer(buffer, pcb_io->quantum);
    cargar_uint8_al_buffer(buffer, pcb_io->flag_int);

    // Registros
    cargar_uint8_al_buffer(buffer, pcb_io->registros->AX);
    cargar_uint8_al_buffer(buffer, pcb_io->registros->BX);
    cargar_uint8_al_buffer(buffer, pcb_io->registros->CX);
    cargar_uint8_al_buffer(buffer, pcb_io->registros->DX);
    cargar_uint32_al_buffer(buffer, pcb_io->registros->EAX);
    cargar_uint32_al_buffer(buffer, pcb_io->registros->EBX);
    cargar_uint32_al_buffer(buffer, pcb_io->registros->ECX);
    cargar_uint32_al_buffer(buffer, pcb_io->registros->EDX);
    cargar_uint32_al_buffer(buffer, pcb_io->registros->SI);
    cargar_uint32_al_buffer(buffer, pcb_io->registros->DI);

    cargar_uint32_al_buffer(buffer, unidades_trabajo);
    cargar_string_al_buffer(buffer, nombre);

    return buffer;
}

void send_io_gen_sleep(int fd, t_pcb* pcb_io, uint32_t unidades_trabajo, char* nombre, uint32_t length) {
    t_buffer *buffer = serializar_io_gen_sleep(pcb_io, unidades_trabajo, nombre, length);
    t_paquete *a_enviar = crear_paquete(IO_GEN_SLEEP, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_io_gen_sleep(int fd, t_pcb* pcb_io, uint32_t* unidades_trabajo, char* nombre) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->offset = 0;

    // Control para recibir el buffer
    int bytes_recibidos = recv(fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
    if (bytes_recibidos != sizeof(uint32_t)) {
        printf("Error: No se recibió el tamaño del buffer completo\n");
        free(paquete->buffer);
        free(paquete);
        return false;
    }

    paquete->buffer->stream = malloc(paquete->buffer->size);
    bytes_recibidos = recv(fd, paquete->buffer->stream, paquete->buffer->size, 0);

    // Datos del pcb
    pcb_io->pid = extraer_uint8_del_buffer(paquete->buffer);
    pcb_io->pc = extraer_uint32_del_buffer(paquete->buffer);
    pcb_io->estado = extraer_char_del_buffer(paquete->buffer);
    pcb_io->quantum = extraer_uint32_del_buffer(paquete->buffer);
    pcb_io->flag_int = extraer_uint8_del_buffer(paquete->buffer);

    // Registros
    pcb_io->registros->AX = extraer_uint8_del_buffer(paquete->buffer);
    pcb_io->registros->BX = extraer_uint8_del_buffer(paquete->buffer);
    pcb_io->registros->CX = extraer_uint8_del_buffer(paquete->buffer);
    pcb_io->registros->DX = extraer_uint8_del_buffer(paquete->buffer);
    pcb_io->registros->EAX = extraer_uint32_del_buffer(paquete->buffer);
    pcb_io->registros->EBX = extraer_uint32_del_buffer(paquete->buffer);
    pcb_io->registros->ECX = extraer_uint32_del_buffer(paquete->buffer);
    pcb_io->registros->EDX = extraer_uint32_del_buffer(paquete->buffer);
    pcb_io->registros->SI = extraer_uint32_del_buffer(paquete->buffer);
    pcb_io->registros->DI = extraer_uint32_del_buffer(paquete->buffer);

    *unidades_trabajo = extraer_uint32_del_buffer(paquete->buffer);

    char* received_nombre = deserializar_string(paquete->buffer);
    strcpy(nombre, received_nombre);
    free(received_nombre);

    eliminar_paquete(paquete);

    return true;
}