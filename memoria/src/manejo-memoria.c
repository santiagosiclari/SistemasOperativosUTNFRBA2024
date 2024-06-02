#include "../include/manejo-memoria.h"

int obtener_marco_asignado(uint8_t pid, int pagina, t_list *tabla_paginas) {
    if (pagina >= list_size(tabla_paginas)) {
        // La página está fuera del rango de la tabla de páginas
        return -1;
    }
    int marco = (intptr_t)list_get(tabla_paginas, pagina);
    log_info(memoria_logger, "Acceso a Tabla de Paginas");
    log_info(memoria_logger, "PID: %d - Pagina: %d - Marco: %d", pid, pagina, marco);
    return marco;
}

int obtener_marco_libre(t_bitarray* marcos_ocupados) {
    for (size_t i = 0; i < bitarray_get_max_bit(marcos_ocupados); i++) {
        if (!bitarray_test_bit(marcos_ocupados, i)) {
            bitarray_set_bit(marcos_ocupados, i);
            return i;
        }
    }
    return -1; // Si no se encuentra un marco libre
}

void liberar_marco(t_bitarray* marcos_ocupados, int marco) {
    bitarray_clean_bit(marcos_ocupados, marco);
}

void* direccion_de_marco(void* espacio_usuario, int marco, size_t tam_frame) {
    return espacio_usuario + (marco * tam_frame);
}

// Para ver si es Out of memory
int contar_marcos_libres(t_bitarray* marcos_ocupados) {
    int count = 0;
    for (size_t i = 0; i < bitarray_get_max_bit(marcos_ocupados); i++) {
        if (!bitarray_test_bit(marcos_ocupados, i)) {
            count++;
        }
    }
    return count;
}

bool escribir_memoria(void* espacio_usuario, uint32_t numero_marco, uint32_t desplazamiento, uint32_t tamanio, void* datos) {
    // Calcula la dirección física dentro del espacio de usuario
    uint32_t direccion_fisica = numero_marco * TAM_PAGINA + desplazamiento;

    // Verificación para evitar sobrepasar el espacio de usuario
    if (direccion_fisica + tamanio > TAM_MEMORIA) {
        return false; // Error: intento de escritura fuera de los límites
    }

    // Escribir en la memoria
    memcpy(espacio_usuario + direccion_fisica, datos, tamanio);

    return true;
}

bool leer_memoria(void* espacio_usuario, uint32_t numero_marco, uint32_t desplazamiento, uint32_t tamanio, void* datos) {
    // Calcula la dirección física dentro del espacio de usuario
    uint32_t direccion_fisica = numero_marco * TAM_PAGINA + desplazamiento;

    // Verificación para evitar sobrepasar el espacio de usuario
    if (direccion_fisica + tamanio > TAM_MEMORIA) {
        return false; // Error: intento de lectura fuera de los límites
    }

    // Leer de la memoria
    memcpy(datos, espacio_usuario + direccion_fisica, tamanio);

    return true;
}

// Funcion para encontrar el minimo
int min(int a, int b) {
    return (a < b) ? a : b;
}