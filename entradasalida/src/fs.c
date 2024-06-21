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

    free(path_archivo_a_crear);
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
int contar_bloques_libres_continuos(t_bitarray* bitmap_bloques, int bloque_inicial, int bloques_necesarios) {
    int bloques_libres = 0;
    int index = bloque_inicial + 1;
    
    while (index < bitarray_get_max_bit(bitmap_bloques)) {
        if (!bitarray_test_bit(bitmap_bloques, index)) {
            bloques_libres++;
            if (bloques_libres >= bloques_necesarios) {
                break; // Ya encontramos suficientes bloques libres
            }
        } else {
            break; // Encontramos un bloque ocupado
        }
        index++;
    }

    return bloques_libres;
}

void ocupar_bloque(t_bitarray* bitmap_bloques, int bloque) {
    bitarray_set_bit(bitmap_bloques, bloque);
}

// Truncate
void truncate_archivo(char* nombre, int tamanio_nuevo, t_bitarray* bitmap_bloques, t_pcb* pcb) {
    // Concatenar nombre con PATH_BASE_DIALFS
    uint32_t MAX_LENGTH = 256;
    char* path_archivo_a_truncar = malloc(MAX_LENGTH);
    strcpy(path_archivo_a_truncar, PATH_BASE_DIALFS);
    strcat(path_archivo_a_truncar, "/");
    strcat(path_archivo_a_truncar, nombre);

    t_config* metadata = config_create(path_archivo_a_truncar);

    // Leer los valores como si fuera config
    int bloque_inicial = config_get_int_value(metadata, "BLOQUE_INICIAL");
    int tamanio_actual = config_get_int_value(metadata, "TAMANIO_ARCHIVO");

    config_destroy(metadata);

    int bloques_actuales = ceil(tamanio_actual / (float)BLOCK_SIZE);
    // Cuanto bloques necesito
    int bloques_necesarios = ceil(tamanio_nuevo / (float)BLOCK_SIZE) - 1; // Restamos 1 porque ya tiene un bloque inicial

    // Logica de cambiar tamaño
    int bloques_libres = contar_bloques_libres(bitmap_blocks);
    if (bloques_libres < bloques_necesarios) {
        log_error(entradasalida_logger, "No hay suficiente espacio para el archivo");
        return;
    }

    log_info(entradasalida_logger, "Tamanio actual: %d", tamanio_actual);
    log_info(entradasalida_logger, "Tamanio nuevo: %d", tamanio_nuevo);
    if(tamanio_actual > tamanio_nuevo) {
        // Disminuir tamaño
        int bloque_final = bloque_inicial + bloques_actuales;
        for (int i = 0; i < bloques_necesarios; i++) {
            liberar_bloque(bitmap_blocks, bloque_final);
            log_info(entradasalida_logger, "Bloque %d liberado", bloque_final);
            bloque_final--;
        }
    } else if(tamanio_actual < tamanio_nuevo) {
        int bloques_libres_continuos = contar_bloques_libres_continuos(bitmap_blocks, bloque_inicial, bloques_necesarios);

        log_info(entradasalida_logger, "Bloques necesarios: %d", bloques_necesarios);
        log_info(entradasalida_logger, "Bloques libres continuos: %d", bloques_libres_continuos);
        if(bloques_libres_continuos < bloques_necesarios) {
            log_info(entradasalida_logger, "PID: %d - Inicio Compactacion.", pcb->pid);
            // Compactar
            log_info(entradasalida_logger, "PID: %d - Fin Compactacion.", pcb->pid);
            usleep(RETRASO_COMPACTACION * 1000);
        } else {
            // Aumentar tamaño
            log_info(entradasalida_logger, "Aumentando tamanio");
            int bloque_final = bloque_inicial + bloques_actuales;
            for (int i = 0; i < bloques_necesarios; i++) {
                bloque_final++;
                ocupar_bloque(bitmap_blocks, bloque_final);
                log_info(entradasalida_logger, "Bloque %d ocupado", bloque_final);
            }
        }
    }

    FILE* archivo_a_truncar = fopen(path_archivo_a_truncar, "w");
    if (!archivo_a_truncar) {
        log_error(entradasalida_logger, "Error: No se pudo crear el archivo a truncar.");
        return;
    }

    // Escribir nuevo tamaño en el archivo
    fprintf(archivo_a_truncar, "BLOQUE_INICIAL=%d\n", bloque_inicial);
    fprintf(archivo_a_truncar, "TAMANIO_ARCHIVO=%d", tamanio_nuevo);
    
    fclose(archivo_a_truncar);
}

// Write
void write_archivo(char* nombre, void* datos, int tamanio_write, int ptr_archivo_write, t_bitarray* bitmap_bloques) {
    // Concatenar nombre con PATH_BASE_DIALFS
    uint32_t MAX_LENGTH = 256;
    char* path_archivo_write = malloc(MAX_LENGTH);
    strcpy(path_archivo_write, PATH_BASE_DIALFS);
    strcat(path_archivo_write, "/");
    strcat(path_archivo_write, nombre);

    // Leo de memoria, espero recibir los datos y lo escribo en el archivo
    FILE* archivo_write = fopen(path_archivo_write, "rb+");
    if(archivo_write == NULL) {
        log_error(entradasalida_logger, "Error al abrir el archivo a escribir");
        return;
    }

    // Moverse al puntero archivo
    if (fseek(archivo_write, ptr_archivo_write, SEEK_SET) != 0) {
        log_error(entradasalida_logger, "Error al moverse al puntero del archivo");
        fclose(archivo_write);
        return;
    }

    // Escribir los datos
    fwrite(datos, 1, tamanio_write, archivo_write);

    fclose(archivo_write);
    free(path_archivo_write);
}

// Read
void read_archivo(char* nombre, int tamanio_read, int dir_fisica_read, int ptr_archivo_read, t_pcb* pcb_fs_read, t_bitarray* bitmap_bloques) {
    // Concatenar nombre con PATH_BASE_DIALFS
    uint32_t MAX_LENGTH = 256;
    char* path_archivo_read = malloc(MAX_LENGTH);
    strcpy(path_archivo_read, PATH_BASE_DIALFS);
    strcat(path_archivo_read, "/");
    strcat(path_archivo_read, nombre);

    // Leo de archivo, envio los datos a escribir en memoria y recibo si la escritura fue ejecutada
    FILE* archivo_read = fopen(path_archivo_read, "rb+");
    if(archivo_read == NULL) {
        log_error(entradasalida_logger, "Error al abrir el archivo a escribir");
        return;
    }

    // Moverse a la posición deseada
    if (fseek(archivo_read, ptr_archivo_read, SEEK_SET) != 0) {
        log_error(entradasalida_logger, "Error al moverse al puntero del archivo");
        fclose(archivo_read);
        free(path_archivo_read);
        return;
    }

    // Escribir los datos
    void* datos_leidos = malloc(tamanio_read);
    fread(datos_leidos, 1, tamanio_read, archivo_read);

    send_escribir_memoria(fd_memoria, pcb_fs_read->pid, dir_fisica_read, datos_leidos, tamanio_read);

    fclose(archivo_read);
    free(path_archivo_read);
}