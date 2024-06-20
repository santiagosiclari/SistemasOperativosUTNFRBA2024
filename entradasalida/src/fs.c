#include "../include/fs.h"

void init_bloques(char* path) {
    // Calcular el tamaño total del archivo
    uint32_t file_size = BLOCK_SIZE * BLOCK_COUNT;

    FILE* archivo_bloques = fopen(path, "wb+");
    if (archivo_bloques == NULL) {
        log_error(entradasalida_logger, "Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    ftruncate(fileno(archivo_bloques), file_size);
    fseek(archivo_bloques, file_size - 1, SEEK_SET);
    fputc('\0', archivo_bloques);

    fclose(archivo_bloques);

    log_info(entradasalida_logger, "Archivo bloques.dat inicializado correctamente.");
}

void init_bitmap(char* path) {
    FILE* arch_bitmap = fopen(path, "wb+");
    if (arch_bitmap == NULL){
        log_error(entradasalida_logger, "Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Redimensionar el archivo para mmap
    fflush(arch_bitmap);
    ftruncate(fileno(arch_bitmap), BLOCK_COUNT / 8);

    // Usar mmap para mapear el archivo a memoria
    void* arch_bitmap_mapped = mmap(NULL, BLOCK_COUNT / 8, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(arch_bitmap), 0);
    if (arch_bitmap_mapped == MAP_FAILED) {
        log_error(entradasalida_logger, "Error al mapear el archivo.");
        fclose(arch_bitmap);
        exit(EXIT_FAILURE);
    }

    memset(arch_bitmap_mapped, 0, BLOCK_COUNT / 8);
    msync(arch_bitmap_mapped, BLOCK_COUNT / 8, MS_SYNC);

    // Para marcar bloques libres u ocupados
    bitmap_blocks = bitarray_create_with_mode(arch_bitmap_mapped, BLOCK_COUNT / 8, LSB_FIRST);

    // munmap(arch_bitmap_mapped, BLOCK_COUNT / 8); // ?
    fclose(arch_bitmap);

    log_info(entradasalida_logger, "Archivo bitmap.dat inicializado correctamente.");
}

// Crear archivo
int primer_bloque_libre(t_bitarray* bitmap_bloques) {
    for (int i = 0; i < bitarray_get_max_bit(bitmap_bloques); i++) {
        if (!bitarray_test_bit(bitmap_bloques, i)) {
            bitarray_set_bit(bitmap_bloques, i);
            return i;
        }
    }
    return -1; // Si no se encuentra un bloque libre
}

void crear_archivo(char* nombre, t_bitarray* bitmap_bloques) {
    // Concatenar nombre con PATH_BASE_DIALFS
    uint32_t MAX_LENGTH = 256;
    char* path_archivo_a_crear = malloc(MAX_LENGTH);
    strcpy(path_archivo_a_crear, PATH_BASE_DIALFS);
    strcat(path_archivo_a_crear, "/");
    strcat(path_archivo_a_crear, nombre);

    int primer_bloque = primer_bloque_libre(bitmap_bloques);

    if (primer_bloque == -1) {
        log_error(entradasalida_logger, "Error: No se encontraron bloques libres.");
        return;
    }

    // Crear archivo de metadata
    FILE *metadata = fopen(path_archivo_a_crear, "w");
    if (!metadata) {
        log_error(entradasalida_logger, "Error: No se pudo crear el archivo de metadata.");
        return;
    }

    // Escribir bloque inicial y tamaño en el archivo
    fprintf(metadata, "BLOQUE_INICIAL=%d\n", primer_bloque);
    fprintf(metadata, "TAMANIO_ARCHIVO=0");
    
    fclose(metadata);

    log_info(entradasalida_logger, "Archivo '%s' creado con bloque inicial %d y tamaño 0.", nombre, primer_bloque);
}

// Borrar archivo
void liberar_bloque(t_bitarray* bitmap_bloques, int bloque) {
    bitarray_clean_bit(bitmap_bloques, bloque);
}

void borrar_archivo(char* nombre, t_bitarray* bitmap_bloques) {
    // Concatenar nombre con PATH_BASE_DIALFS
    uint32_t MAX_LENGTH = 256;
    char* path_archivo_a_borrar = malloc(MAX_LENGTH);
    strcpy(path_archivo_a_borrar, PATH_BASE_DIALFS);
    strcat(path_archivo_a_borrar, "/");
    strcat(path_archivo_a_borrar, nombre);

    t_config* metadata = config_create(path_archivo_a_borrar);

    // Leer los valores como si fuera config
    int bloque_inicial = config_get_int_value(metadata, "BLOQUE_INICIAL");
    int tamanio_archivo = config_get_int_value(metadata, "TAMANIO_ARCHIVO");

    // Cuantos bloques tiene el archivo
    int bloques_ocupados = ceil(tamanio_archivo / (float)BLOCK_SIZE);

    // Liberar los bloques en el bitmap
    for (int i = 0; i < bloques_ocupados; i++) {
        liberar_bloque(bitmap_bloques, bloque_inicial + i);
    }

    // Eliminar el archivo del FS
    if (remove(path_archivo_a_borrar) != 0) {
        log_error(entradasalida_logger, "No se pudo eliminar el archivo de metadata.");
    } else {
        log_info(entradasalida_logger, "Archivo '%s' borrado y bloques liberados.", nombre);
    }

    config_destroy(metadata);
    free(path_archivo_a_borrar);
}

// Para ver si hay suficientes bloques libres
int contar_bloques_libres(t_bitarray* bitmap_bloques) {
    int count = 0;
    for (int i = 0; i < bitarray_get_max_bit(bitmap_bloques); i++) {
        if (!bitarray_test_bit(bitmap_bloques, i)) {
            count++;
        }
    }
    return count;
}

// Ver si hay que compactar
int bloques_libres_continuos(t_bitarray* bitmap_bloques, int bloque_inicial, int bloques_necesarios) {
    int bloques_libres = 0;
    int index = bloque_inicial;
    
    while (index < bitarray_get_max_bit(bitmap_bloques)) {
        if (!bitarray_test_bit(bitmap_bloques, index)) {
            bloques_libres++;
            if (bloques_libres >= bloques_necesarios) {
                break; // Ya encontramos suficientes bloques libres
            }
        } else {
            break; // Encontramos un bloque ocupado, detenemos la búsqueda
        }
        index++;
    }

    return bloques_libres;
}