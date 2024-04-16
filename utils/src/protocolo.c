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

bool send_handshake(t_log* logger,int fd,int32_t hs,const char* connection_name)
{
    size_t bytes;

    int32_t handshake = hs;
    int32_t result;

    bytes = send(fd, &handshake, sizeof(int32_t), 0);
    bytes = recv(fd, &result, sizeof(int32_t), MSG_WAITALL);

    if (result == 0) {
        log_info(logger, "Handshake OK de %s", connection_name);
    } else {
        log_error(logger, "Handshake ERROR de %s, el error es %s (%d)", connection_name, strerror(errno), errno);
    }
    return true;
}

bool recv_handshake(int fd)
{
    size_t bytes;
    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    bytes = recv(fd, &handshake, sizeof(int32_t), MSG_WAITALL);
    if (handshake == 1) {
        bytes = send(fd, &resultOk, sizeof(int32_t), 0);
    } else {
        bytes = send(fd, &resultError, sizeof(int32_t), 0);
    }

    return true;
}