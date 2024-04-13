#include "../include/protocolo.h"

// Funciones de send y recv del protocolo

// MENSAJE
static void* serializar_mensaje(uint8_t num) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint8_t));

    op_code cop = MENSAJE;
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &num, sizeof(uint8_t));
    return stream;
}

static void deserializar_mensaje(void* stream, uint8_t* num) {
    memcpy(num, stream, sizeof(uint8_t));
}

bool send_mensaje(int fd, uint8_t num) {
    size_t size = sizeof(op_code) + sizeof(uint8_t);
    void* stream = serializar_mensaje(num);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_mensaje(int fd, uint8_t* num) {
    size_t size = sizeof(uint8_t);
    void* stream = malloc(size);

    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    deserializar_mensaje(stream, num);

    free(stream);
    return true;
}