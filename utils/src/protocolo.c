#include "../include/protocolo.h"

// Funciones de send y recv del protocolo
// Handshake
bool send_handshake(t_log* logger, int fd, int32_t handshake, const char* conexion) {
    size_t bytes;
    int32_t result;

    bytes = send(fd, &handshake, sizeof(int32_t), 0);
    bytes = recv(fd, &result, sizeof(int32_t), MSG_WAITALL);

    if (result == 0 && bytes != 0) {
        log_info(logger, "Handshake OK de %s\n", conexion);
        return true;
    } else {
        log_error(logger, "Handshake ERROR de %s\n", conexion);
        return false;
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
    } else {
        bytes = send(fd, &resultError, sizeof(int32_t), 0);
        return false;
    }
}

// PCB
static void* serializar_pcb(size_t* size, t_pcb* pcb) {

    *size = 
        sizeof(uint8_t) +    // pid
        sizeof(uint32_t) +   // pc
        sizeof(char) +       // estado
        sizeof(uint8_t) +    // quantum
        sizeof(t_registros); // registros serializados

    void* stream = malloc(*size);

    size_t offset = 0;

    memcpy(stream + offset, &pcb->pid, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    memcpy(stream + offset, &pcb->pc, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &pcb->estado, sizeof(char));
    offset += sizeof(char);

    memcpy(stream + offset, &pcb->quantum, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    memcpy(stream+ offset, &pcb->registros, sizeof(t_registros));
    offset += sizeof(t_registros);

    return stream;
}

static void deserializar_pcb(void* stream, t_pcb** pcb) {
    size_t size_pcb;
    memcpy(&size_pcb, stream, sizeof(size_t));

    t_pcb* r_pcb = malloc(size_pcb);
    memcpy(r_pcb, stream+sizeof(size_t), size_pcb);
    *pcb = r_pcb;
}

bool send_pcb(int fd, t_pcb* pcb) {
    size_t size;
    void* stream = serializar_pcb(&size, pcb);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_pcb(int fd, t_pcb** pcb) {
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
        return false;

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_pcb(stream, pcb);

    free(stream);
    return true;
}

// Contexto de ejecucion
static void* serializar_contexto_de_ejecucion(size_t* size, t_registros* registros, uint32_t pc) {

    *size = 
        sizeof(uint32_t) +   // pc
        sizeof(t_registros); // registros serializados

    void* stream = malloc(*size);

    size_t offset = 0;

    memcpy(stream + offset, &pc, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream+ offset, &registros, sizeof(t_registros));
    offset += sizeof(t_registros);

    return stream;
}

static void deserializar_contexto_de_ejecucion(void* stream, t_registros** registros, uint32_t* pc) {
    size_t offset = 0;

    memcpy(&pc, stream + offset, sizeof(uint32_t)); // pc
    offset += sizeof(uint32_t);

    memcpy(&registros, stream + offset, sizeof(t_registros)); // registros
}

bool send_contexto_de_ejecucion(int fd, t_registros* registros, uint32_t pc) {
    size_t size;
    void* stream = serializar_contexto_de_ejecucion(&size, registros, pc);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_contexto_de_ejecucion(int fd, t_registros** registros, uint32_t* pc) {
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
        return false;

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_contexto_de_ejecucion(stream, registros, pc);

    free(stream);
    return true;
}

// INICIAR_PROCESO
static void* serializar_iniciar_proceso(size_t* size, char* path) {
    size_t size_path = strlen(path) + 1;
    *size =
          sizeof(op_code)   // cop
        + sizeof(size_t)    // total
        + sizeof(size_t)    // size de char* path
        + size_path;        // char* path
    size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);

    void* stream = malloc(*size);

    op_code cop = INICIAR_PROCESO;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(size_t), &size_path, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(size_t)*2, path, size_path);

    return stream;
}

static void deserializar_iniciar_proceso(void* stream, char** path) {
    // Path
    size_t size_path;
    memcpy(&size_path, stream, sizeof(size_t));

    char* r_path = malloc(size_path);
    memcpy(r_path, stream+sizeof(size_t), size_path);
    *path = r_path;
}

bool send_iniciar_proceso(int fd, char* path) {
    size_t size;
    void* stream = serializar_iniciar_proceso(&size, path);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_iniciar_proceso(int fd, char** path) {
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
        return false;

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_iniciar_proceso(stream, path);

    free(stream);
    return true;
}