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

// PC -> Program Counter
t_buffer* serializar_pc(uint32_t pc) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size =
        sizeof(uint32_t); // pc

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint32_al_buffer(buffer, pc);

    return buffer;
}

uint32_t deserializar_pc(t_buffer* buffer, uint32_t pc) {
    pc = extraer_uint32_del_buffer(buffer); // PC
    return pc;
}

void send_pc(int fd, uint32_t pc) {
    t_buffer *buffer = serializar_pc(pc);
    t_paquete *a_enviar = crear_paquete(RECIBIR_PC, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_pc(int fd, uint32_t pc) {
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

    deserializar_pc(paquete->buffer, pc);

    eliminar_paquete(paquete);

    return true;
}

t_buffer* serializar_uint8(uint8_t entero) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint8_t);

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint8_al_buffer(buffer, entero);

    return buffer;
}

uint8_t deserializar_uint8(t_buffer* buffer, uint8_t entero) {
    entero = extraer_uint8_del_buffer(buffer);
    return entero;
}

// PID
void send_pid(int fd, uint8_t pid) {
    t_buffer *buffer = serializar_uint8(pid);
    t_paquete *a_enviar = crear_paquete(RECIBIR_PID, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_pid(int fd, uint8_t pid) {
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

    deserializar_uint8(paquete->buffer, pid);

    eliminar_paquete(paquete);

    return true;
}

// Size instrucciones
void send_size_instrucciones(int fd, uint8_t size) {
    t_buffer *buffer = serializar_uint8(size);
    t_paquete *a_enviar = crear_paquete(RECIBIR_SIZE_INSTRUCCIONES, buffer);
    enviar_paquete(a_enviar, size);
    eliminar_paquete(a_enviar);
}

bool recv_size_instrucciones(int fd, uint8_t size) {
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

    deserializar_uint8(paquete->buffer, size);

    eliminar_paquete(paquete);

    return true;
}

// PCB
// Serializar PCB
t_buffer* serializar_pcb(t_pcb* pcb) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size =
        sizeof(uint8_t) +    // pid
        sizeof(uint32_t) +   // pc
        sizeof(char) +       // estado
        sizeof(uint32_t) +   // quantum
        sizeof(t_registros); // tamaño de registros

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint8_al_buffer(buffer, pcb->pid);
    cargar_uint32_al_buffer(buffer, pcb->pc);
    cargar_char_al_buffer(buffer, pcb->estado);
    cargar_uint32_al_buffer(buffer, pcb->quantum);

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

// Contexto de ejecucion
// Deserializar Registros
t_buffer* serializar_contexto_de_ejecucion(t_registros* registros, uint32_t pc) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size =
        sizeof(uint32_t) +   // pc
        sizeof(t_registros); // tamaño de registros

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    // PC
    cargar_uint32_al_buffer(buffer, pc);

    // Registros
    cargar_uint8_al_buffer(buffer, registros->AX);
    cargar_uint8_al_buffer(buffer, registros->BX);
    cargar_uint8_al_buffer(buffer, registros->CX);
    cargar_uint8_al_buffer(buffer, registros->DX);
    cargar_uint32_al_buffer(buffer, registros->EAX);
    cargar_uint32_al_buffer(buffer, registros->EBX);
    cargar_uint32_al_buffer(buffer, registros->ECX);
    cargar_uint32_al_buffer(buffer, registros->EDX);
    cargar_uint32_al_buffer(buffer, registros->SI);
    cargar_uint32_al_buffer(buffer, registros->DI);

    return buffer;
}

// Deserializar PC ya esta hecho antes, falta deserializar t_registros
t_registros* deserializar_registros(t_buffer *buffer, t_registros *registros) {
    // Registros
    registros->AX = extraer_uint8_del_buffer(buffer);
    registros->BX = extraer_uint8_del_buffer(buffer);
    registros->CX = extraer_uint8_del_buffer(buffer);
    registros->DX = extraer_uint8_del_buffer(buffer);
    registros->EAX = extraer_uint32_del_buffer(buffer);
    registros->EBX = extraer_uint32_del_buffer(buffer);
    registros->ECX = extraer_uint32_del_buffer(buffer);
    registros->EDX = extraer_uint32_del_buffer(buffer);
    registros->SI = extraer_uint32_del_buffer(buffer);
    registros->DI = extraer_uint32_del_buffer(buffer);

    return registros;
}

void send_contexto_de_ejecucion(int fd, t_registros *registros, uint32_t pc) {
    t_buffer *buffer = serializar_contexto_de_ejecucion(registros, pc);
    t_paquete *a_enviar = crear_paquete(RECIBIR_CONTEXTO_DE_EJEC, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_contexto_de_ejecucion(int fd, t_registros *registros, uint32_t pc) {
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

    deserializar_pc(paquete->buffer, pc);
    deserializar_registros(paquete->buffer, registros);

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
void send_iniciar_proceso(int fd, char* path, uint32_t length) {
    t_buffer *buffer = serializar_string(path, length);
    t_paquete *a_enviar = crear_paquete(INICIAR_PROCESO, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);
}

bool recv_iniciar_proceso(int fd, char* path) {
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