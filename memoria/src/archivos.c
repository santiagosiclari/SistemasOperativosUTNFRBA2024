#include "../include/archivos.h"

#define MAX_LENGTH 128 

void leer_archivo(char* path) {
    FILE* archivo;
    // Abro el archivo
    archivo = fopen(path, "rt");

    // Leo el archivo
    if (archivo == NULL) {
        perror("Error abriendo archivo");
        return;
    }

    char* instruccion = malloc(MAX_LENGTH); // Espacio para cada línea
    while (fgets(instruccion, MAX_LENGTH, archivo) != NULL) { // Leer línea por línea
        // Puede leer pero FALTA guardar las instrucciones en listas y enviar una por una a CPU
        log_info(memoria_logger, "%s", instruccion); // Imprimir la línea
    }

    free(instruccion);

    fclose(archivo);
}