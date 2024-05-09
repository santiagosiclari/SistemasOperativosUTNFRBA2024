#include "../include/protocolo.h"

// Funciones de send y recv del protocolo
// Handshake
void send_handshake(t_log* logger, int fd, int32_t handshake, const char *conexion)
{
    size_t bytes;
    int32_t result;

    bytes = send(fd, &handshake, sizeof(int32_t), 0);
    bytes = recv(fd, &result, sizeof(int32_t), MSG_WAITALL);

    if (result == 0 && bytes != 0)
    {
        log_info(logger, "Handshake OK de %s\n", conexion);
    }
    else
    {
        log_error(logger, "Handshake ERROR de %s\n", conexion);
    }
}

bool recv_handshake(int fd, int32_t handshakeModulo)
{
    size_t bytes;
    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    bytes = recv(fd, &handshake, sizeof(int32_t), MSG_WAITALL);
    if (handshake == handshakeModulo && bytes != 0)
    {
        bytes = send(fd, &resultOk, sizeof(int32_t), 0);
        return true;
    }
    else
    {
        bytes = send(fd, &resultError, sizeof(int32_t), 0);
        return false;
    }
}

// PCB
// Serializar PCB
t_buffer* serializar_pcb(t_pcb* pcb) {
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size =
        sizeof(uint8_t) +    // pid
        sizeof(uint32_t) +   // pc
        sizeof(char) +       // estado
        sizeof(int) +        // quantum
        sizeof(t_registros); // tamaño de registros

    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);

    cargar_uint8_al_buffer(buffer, pcb->pid);
    cargar_uint32_al_buffer(buffer, pcb->pc);
    cargar_char_al_buffer(buffer, pcb->estado);
    cargar_int_al_buffer(buffer, pcb->quantum);

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
    pcb->quantum = extraer_int_del_buffer(buffer);

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

bool send_pcb(int fd, t_pcb *pcb) {
    t_buffer *buffer = serializar_pcb(pcb);
    t_paquete *a_enviar = crear_paquete(RECIBIR_PCB, buffer);
    enviar_paquete(a_enviar, fd);
    eliminar_paquete(a_enviar);

    return true;
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
static void *serializar_contexto_de_ejecucion(size_t *size, t_registros *registros, uint32_t pc)
{

    *size =
        sizeof(uint32_t) +   // pc
        sizeof(t_registros); // registros serializados

    void *stream = malloc(*size);

    size_t offset = 0;

    memcpy(stream + offset, &pc, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &registros, sizeof(t_registros));
    offset += sizeof(t_registros);

    return stream;
}

static void deserializar_contexto_de_ejecucion(void *stream, t_registros **registros, uint32_t *pc)
{
    size_t offset = 0;

    memcpy(&pc, stream + offset, sizeof(uint32_t)); // pc
    offset += sizeof(uint32_t);

    memcpy(&registros, stream + offset, sizeof(t_registros)); // registros
}

bool send_contexto_de_ejecucion(int fd, t_registros *registros, uint32_t pc)
{
    size_t size;
    void *stream = serializar_contexto_de_ejecucion(&size, registros, pc);
    if (send(fd, stream, size, 0) != size)
    {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_contexto_de_ejecucion(int fd, t_registros **registros, uint32_t *pc)
{
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
        return false;

    void *stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload)
    {
        free(stream);
        return false;
    }

    deserializar_contexto_de_ejecucion(stream, registros, pc);

    free(stream);
    return true;
}

// Char* (String)
static void *serializar_string(size_t *size, char *string)
{
    size_t size_string = strlen(string) + 1;
    *size =
        sizeof(size_t) // size de char*
        + size_string; // char*

    void *stream = malloc(*size);

    memcpy(stream, &size_string, sizeof(size_t));
    memcpy(stream + sizeof(size_t), string, size_string);

    return stream;
}

static void deserializar_string(void *stream, char **string)
{
    // String
    size_t size_string;
    memcpy(&size_string, stream, sizeof(size_t));

    char *r_string = malloc(size_string);
    memcpy(r_string, stream + sizeof(size_t), size_string);
    *string = r_string;
}

bool send_string(int fd, char *string)
{
    size_t size;
    void *stream = serializar_string(&size, string);
    if (send(fd, stream, size, 0) != size)
    {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_string(int fd, char **string)
{
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
    {
        return false;
    }

    void *stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload)
    {
        free(stream);
        return false;
    }

    deserializar_string(stream, string);

    free(stream);
    return true;
}

// PC -> Program Counter
static void *serializar_pc(size_t *size, uint32_t pc)
{
    *size =
        sizeof(op_code)     // cop
        + sizeof(size_t)    // total
        + sizeof(uint32_t); // size de uint32_t
    size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);

    void *stream = malloc(*size);

    op_code cop = RECIBIR_PC;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream + sizeof(op_code), &size_payload, sizeof(size_t));
    memcpy(stream + sizeof(op_code) + sizeof(size_t), &pc, sizeof(uint32_t));

    return stream;
}

static void deserializar_pc(void *stream, uint32_t *pc)
{
    memcpy(pc, stream, sizeof(uint32_t)); // PC
}

bool send_pc(int fd, uint32_t pc)
{
    size_t size;
    void *stream = serializar_pc(&size, pc);
    if (send(fd, stream, size, 0) != size)
    {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_pc(int fd, uint32_t *pc)
{
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
    {
        return false;
    }

    void *stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload)
    {
        free(stream);
        return false;
    }

    deserializar_pc(stream, pc);

    free(stream);
    return true;
}

// PID
static void *serializar_pid(size_t *size, uint8_t pid)
{
    *size = sizeof(uint8_t); // PID
    void *stream = malloc(*size);
    memcpy(stream, &pid, sizeof(uint8_t));
    return stream;
}

static void deserializar_pid(void *stream, uint8_t *pid)
{
    memcpy(pid, stream, sizeof(uint8_t)); // PID
}

bool send_pid(int fd, uint8_t pid)
{
    size_t size;
    void *stream = serializar_pid(&size, pid);
    if (send(fd, stream, size, 0) != size)
    {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_pid(int fd, uint8_t *pid)
{
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
    {
        return false;
    }

    void *stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload)
    {
        free(stream);
        return false;
    }

    deserializar_pid(stream, pid);

    free(stream);
    return true;
}

// INICIAR_PROCESO
static void *serializar_iniciar_proceso(size_t *size, char *path)
{
    size_t size_path = strlen(path) + 1;
    *size =
        sizeof(op_code)  // cop
        + sizeof(size_t) // total
        + sizeof(size_t) // size de char* path
        + size_path;     // char* path
    size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);

    void *stream = malloc(*size);

    op_code cop = INICIAR_PROCESO;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream + sizeof(op_code), &size_payload, sizeof(size_t));
    memcpy(stream + sizeof(op_code) + sizeof(size_t), &size_path, sizeof(size_t));
    memcpy(stream + sizeof(op_code) + sizeof(size_t) * 2, path, size_path);

    return stream;
}

static void deserializar_iniciar_proceso(void *stream, char **path)
{
    size_t size_path; // Path
    memcpy(&size_path, stream, sizeof(size_t));

    char *r_path = malloc(size_path);
    memcpy(r_path, stream + sizeof(size_t), size_path);
    *path = r_path;
}

bool send_iniciar_proceso(int fd, char *path)
{
    size_t size;
    void *stream = serializar_iniciar_proceso(&size, path);
    if (send(fd, stream, size, 0) != size)
    {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_iniciar_proceso(int fd, char **path)
{
    size_t size_payload;
    if (recv(fd, &size_payload, sizeof(size_t), 0) != sizeof(size_t))
        return false;

    void *stream = malloc(size_payload);
    if (recv(fd, stream, size_payload, 0) != size_payload)
    {
        free(stream);
        return false;
    }

    deserializar_iniciar_proceso(stream, path);

    free(stream);
    return true;
}