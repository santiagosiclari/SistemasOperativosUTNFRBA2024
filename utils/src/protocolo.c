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
        log_error(logger, "Handshake ERROR de %s.\n", conexion);
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