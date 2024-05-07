#include "../include/protocolo.h"

// Funciones de send y recv del protocolo
// Handshake
void send_handshake(t_log* logger, int fd, int32_t handshake, const char* conexion) {
    size_t bytes;
    int32_t result;

    bytes = send(fd, &handshake, sizeof(int32_t), 0);
    bytes = recv(fd, &result, sizeof(int32_t), MSG_WAITALL);

    if (result == 0 && bytes != 0) {
        log_info(logger, "Handshake OK de %s\n", conexion);
    } else {
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
    } else {
        bytes = send(fd, &resultError, sizeof(int32_t), 0);
        return false;
    }
}

// PCB
// Serializar t_registros
static void* serializar_registros(size_t* size, t_registros* registros) {
    *size = sizeof(t_registros);
    void* stream = malloc(*size);
    memcpy(stream, registros, *size);
    return stream;
}

// Deserializar t_registros
static void deserializar_registros(void* stream, t_registros** registros) {
    *registros = (t_registros*)malloc(sizeof(t_registros));
    memcpy(*registros, stream, sizeof(t_registros));
}

// Serializar PCB
static void* serializar_pcb(size_t* size, t_pcb* pcb) {
    size_t size_registros;
    void* serialized_registros = serializar_registros(&size_registros, pcb->registros);

    *size = 
            sizeof(op_code) +   // Para el op_code
            sizeof(uint8_t) +   // pid
            sizeof(uint32_t) +  // pc
            sizeof(char) +      // estado
            sizeof(uint8_t) +   // quantum
            sizeof(size_t) +    // Tamaño de registros
            size_registros;     // Registros serializados

    void* stream = malloc(*size);

    size_t offset = 0;
    op_code cop = RECIBIR_PCB;
    memcpy(stream + offset, &cop, sizeof(op_code)); offset += sizeof(op_code); // op_code
    memcpy(stream + offset, &pcb->pid, sizeof(uint8_t)); offset += sizeof(uint8_t);
    memcpy(stream + offset, &pcb->pc, sizeof(uint32_t)); offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->estado, sizeof(char)); offset += sizeof(char);
    memcpy(stream + offset, &pcb->quantum, sizeof(uint8_t)); offset += sizeof(uint8_t);
    memcpy(stream + offset, &size_registros, sizeof(size_t)); offset += sizeof(size_t);
    memcpy(stream + offset, serialized_registros, size_registros);

    free(serialized_registros);

    return stream;
}

// Deserializar PCB
static void deserializar_pcb(void* stream, t_pcb** pcb) {
    *pcb = (t_pcb*)malloc(sizeof(t_pcb));

    size_t offset = 0;
    op_code received_op;
    memcpy(&received_op, stream + offset, sizeof(op_code)); offset += sizeof(op_code);

    if (received_op != RECIBIR_PCB) {
        fprintf(stderr, "Error: Operación desconocida al deserializar PCB.\n");
        free(*pcb);
        *pcb = NULL;
        return;
    }

    memcpy(&(*pcb)->pid, stream + offset, sizeof(uint8_t)); offset += sizeof(uint8_t);
    memcpy(&(*pcb)->pc, stream + offset, sizeof(uint32_t)); offset += sizeof(uint32_t);
    memcpy(&(*pcb)->estado, stream + offset, sizeof(char)); offset += sizeof(char);
    memcpy(&(*pcb)->quantum, stream + offset, sizeof(uint8_t)); offset += sizeof(uint8_t);

    size_t size_registros;
    memcpy(&size_registros, stream + offset, sizeof(size_t)); offset += sizeof(size_t);

    void* serialized_registros = malloc(size_registros);
    memcpy(serialized_registros, stream + offset, size_registros);

    deserializar_registros(serialized_registros, &(*pcb)->registros);

    free(serialized_registros);
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
    ssize_t received;

    // Recibir el tamaño del paquete
    received = recv(fd, &size_payload, sizeof(size_payload), MSG_WAITALL);

    if (received != sizeof(size_payload)) {
        fprintf(stderr, "Error: Tamaño del paquete no recibido correctamente. Recibido: %ld\n", received);
        return false;
    }

    // Asegurarse de que el tamaño recibido tenga sentido
    if (size_payload <= 0) {
        fprintf(stderr, "Error: Tamaño del paquete inválido: %ld\n", size_payload);
        return false;
    }

    // Recibir el contenido del PCB
    void* stream = malloc(size_payload);
    received = recv(fd, stream, size_payload, MSG_WAITALL);

    if (received != size_payload) {
        fprintf(stderr, "Error: No se recibió todo el paquete. Recibido: %ld, esperado: %ld\n", received, size_payload);
        free(stream);
        return false;
    }

    // Deserializar el PCB
    deserializar_pcb(stream, pcb);

    free(stream);

    if (*pcb == NULL) {
        fprintf(stderr, "Error: PCB deserializado es NULL.\n");
        return false;
    }

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

// Char* (String)
static void* serializar_string(size_t* size, char* string) {
    size_t size_string = strlen(string) + 1;
    *size =
        sizeof(size_t)   // size de char*
        + size_string;   // char*
    
    void* stream = malloc(*size);

    memcpy(stream, &size_string, sizeof(size_t));
    memcpy(stream+sizeof(size_t), string, size_string);

    return stream;
}

static void deserializar_string(void* stream, char** string) {
    // String
    size_t size_string;
    memcpy(&size_string, stream, sizeof(size_t));

    char* r_string = malloc(size_string);
    memcpy(r_string, stream+sizeof(size_t), size_string);
    *string = r_string;
}

bool send_string(int fd, char* string) {
    size_t size;
    void* stream = serializar_string(&size, string);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_string(int fd, char** string) {
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_string(stream, string);

    free(stream);
    return true;
}

// PC -> Program Counter
static void* serializar_pc(size_t* size, uint32_t pc) {
    *size =
          sizeof(op_code)   // cop
        + sizeof(size_t)    // total
        + sizeof(uint32_t);  // size de uint32_t
    size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);

    void* stream = malloc(*size);

    op_code cop = RECIBIR_PC;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t));
    memcpy(stream+sizeof(op_code)+sizeof(size_t), &pc, sizeof(uint32_t));

    return stream;
}

static void deserializar_pc(void* stream, uint32_t* pc) {
    memcpy(pc, stream, sizeof(uint32_t)); // PC
}

bool send_pc(int fd, uint32_t pc) {
    size_t size;
    void* stream = serializar_pc(&size, pc);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_pc(int fd, uint32_t* pc) {
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_pc(stream, pc);

    free(stream);
    return true;
}

// PID
static void* serializar_pid(size_t* size, uint8_t pid) {
    *size = sizeof(uint8_t); // PID
    void* stream = malloc(*size);
    memcpy(stream, &pid, sizeof(uint8_t)); 
    return stream;
}

static void deserializar_pid(void* stream, uint8_t* pid) {
    memcpy(pid, stream, sizeof(uint8_t)); // PID
}

bool send_pid(int fd, uint8_t pid) {
    size_t size;
    void* stream = serializar_pid(&size, pid);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_pid(int fd, uint8_t* pid) {
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }

    void* stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload) {
        free(stream);
        return false;
    }

    deserializar_pid(stream, pid);

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
    size_t size_path; // Path
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