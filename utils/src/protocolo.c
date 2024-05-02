#include "../include/protocolo.h"

// Funciones de buffer y paquete
t_buffer* crear_buffer() {
    t_buffer* un_buffer = malloc(sizeof(t_buffer)); 
    un_buffer->size = 0; 
    un_buffer->stream = NULL; 
    return un_buffer;
}

void cargar_choclo_al_buffer(t_buffer* buffer, void* choclo, int size_choclo) {
    if(buffer->size == 0) {
        buffer->stream = malloc(sizeof(int) + size_choclo);
        memcpy(buffer->stream, &size_choclo, sizeof(int));
        memcpy(buffer->stream + sizeof(int), choclo, size_choclo);
    } else {
        buffer->stream = realloc(buffer->stream, buffer->size + sizeof(int) + size_choclo);
        memcpy(buffer->stream + buffer->size, &size_choclo, sizeof(int));
        memcpy(buffer->stream + buffer->size + sizeof(int), choclo, size_choclo);
    }
    buffer->size += sizeof(int);
    buffer->size += size_choclo;
}

void* extraer_choclo_del_buffer(t_buffer* buffer) {
    // Falta analizar errores
    if(buffer->size == 0 || buffer->size < 0) {
        perror("ERROR con el choclo del buffer");
        exit(EXIT_FAILURE);
    }

    int size_choclo;
    memcpy(&size_choclo, buffer->stream, sizeof(int));
    void* choclo = malloc(size_choclo);
    memcpy(choclo, buffer->stream + sizeof(int), size_choclo);

    int nuevo_size = buffer->size - sizeof(int) - size_choclo;

    if(nuevo_size == 0) {
        buffer->size = 0;
        free(buffer->stream);
        buffer->stream = NULL;
        return choclo;
    }

    if(nuevo_size < 0) {
        perror("ERROR con el choclo del buffer");
        exit(EXIT_FAILURE);
    }

    void* nuevo_stream = malloc(nuevo_size);
    memcpy(nuevo_stream, buffer->stream + sizeof(int) + size_choclo, nuevo_size);
    free(buffer->stream);
    buffer->size = nuevo_size;
    buffer->stream = nuevo_stream;

    return choclo;
}

void cargar_string_al_buffer(t_buffer* buffer, char* string) {
    cargar_choclo_al_buffer(buffer, string, strlen(string) + 1);
}

char* extraer_string_del_buffer(t_buffer* buffer) {
    char* string = extraer_choclo_del_buffer(buffer);
    return string;
}

void destruir_buffer(t_buffer* buffer) {
    if(buffer->stream != NULL) {
        free(buffer->stream);
    }

    free(buffer);
}

t_paquete* crear_paquete(op_code cod_op, t_buffer* buffer) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = cod_op;
    paquete->buffer = buffer;
    return paquete;
}

void* serializar_paquete(t_paquete* paquete, int bytes) {
    void * magic = malloc(bytes);
    int desplazamiento = 0;

    memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
    desplazamiento+= paquete->buffer->size;

    return magic;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente) {
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void* a_enviar = serializar_paquete(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete) {
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

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

// Instrucciones de CPU
static void* serializar_set_instruccion(uint8_t registro) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint8_t));

    op_code cop = SET;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &registro, sizeof(uint8_t));
    return stream;
}

static void deserializar_set_instruccion(void* stream, uint8_t* registro) {
    memcpy(registro, stream, sizeof(uint8_t));
}

bool send_set_instruccion(int fd, uint8_t registro) {
    size_t size = sizeof(op_code) + sizeof(uint8_t);
    void* stream = serializar_set_instruccion(registro);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_set_instruccion(int fd, uint8_t* registro) {
    size_t size = sizeof(uint8_t);
    void* stream = malloc(size);

    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    deserializar_set_instruccion(stream, registro);

    free(stream);
    return true;
}