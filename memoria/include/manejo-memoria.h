#ifndef MANEJO_MEMORIA_H_
#define MANEJO_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include "logs.h"
#include "configs.h"

int obtener_marco_asignado(uint8_t pid, int pagina, t_list *tabla_paginas);
int obtener_marco_libre(t_bitarray* marcos_ocupados);
void liberar_marco(t_bitarray* marcos_ocupados, int marco);
void* direccion_de_marco(void* espacio_usuario, int marco, size_t tam_frame);
int contar_marcos_libres(t_bitarray* marcos_ocupados);
bool escribir_memoria(void* espacio_usuario, uint32_t numero_marco, uint32_t desplazamiento, uint32_t tamanio, void* datos);
bool leer_memoria(void* espacio_usuario, uint32_t numero_marco, uint32_t desplazamiento, uint32_t tamanio, void* datos);

int min(int a, int b);

#endif