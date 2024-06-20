#ifndef FS_H_
#define FS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "configs.h"
#include "logs.h"

#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/file-descriptors.h"

extern t_bitarray* bitmap_blocks;

void init_bloques(char* path);
void init_bitmap(char* path);
int primer_bloque_libre(t_bitarray* bitmap_bloques);
void crear_archivo(char* nombre, t_bitarray* bitmap_bloques);
void liberar_bloque(t_bitarray* bitmap_bloques, int bloque);
void borrar_archivo(char* nombre, t_bitarray* bitmap_bloques);
int contar_bloques_libres(t_bitarray* bitmap_bloques);
int bloques_libres_continuos(t_bitarray* bitmap_bloques, int bloque_inicial, int bloques_necesarios);

#endif