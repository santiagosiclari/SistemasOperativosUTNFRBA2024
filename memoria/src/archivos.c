#include "../include/archivos.h"

#define MAX_LENGTH 128 

t_list* instrucciones;

void print_element(void* element) {
    char* str = (char*)element;
    log_info(memoria_logger, "%s", str); // Cambio a "%s" para evitar caracteres no deseados
}

void print_list(t_list* list) {
    list_iterate(list, print_element);
}

t_list* leer_archivo(char* path) {
    FILE* archivo;
    archivo = fopen(path, "rt");

    if (archivo == NULL) {
        perror("Error abriendo archivo");
        exit(EXIT_FAILURE);
    }

    instrucciones = list_create(); // Crear la lista para guardar las instrucciones
    char linea[MAX_LENGTH]; // Usar un buffer para leer líneas
    int contador = 0;

    while (fgets(linea, MAX_LENGTH, archivo) != NULL) { // Leer línea por línea
		log_info(memoria_logger, "Prueba para ver si entra al while");
        // Crea una nueva cadena para cada instrucción y la copia desde `linea`
        char* instruccion = malloc(strlen(linea) + 1); // +1 para el terminador nulo
        strcpy(instruccion, linea); // Copiar la línea leída a la nueva memoria
        list_add(instrucciones, instruccion); // Agregar a la lista
        contador++;
    }

    log_info(memoria_logger, "Prueba para ver si sale del while");

    fclose(archivo); // Cerrar el archivo
    return instrucciones; // Devolver la lista de instrucciones
}