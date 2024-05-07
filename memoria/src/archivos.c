#include "../include/archivos.h"

void leer_archivo(char* path) {
    FILE* archivo;
    // Abro el archivo
    archivo = fopen(path, "rt");

    // Leo el archivo


    fclose(archivo);
}