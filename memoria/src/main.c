#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>

#include <main.h>


#include <main.h>
int main(int argc, char* argv[]) {
    //decir_hola("Memoria");

memoria_logger = log_create(,,,LOG_LEVEL_INFO);
if(memoria_logger == NULL){
    perror("Algo raro paso con el log. No se pudo crear o encontrar el archivo.");
    exit(EXIT_FAILURE);
}

memoria_debug = log_create(,,,LOG_LEVEL_TRACE);
if(memoria_debug == NULL){
    perror("Algo raro paso con el log. No se pudo crear o encontrar el archivo.");
    exit(EXIT_FAILURE);
}

memoria_config = config_create("/home/utnso/tp-2024-1c-ChacoForSystem/memoria/memoria.config");
if (memoria_config == NULL)
{
    perror("Error al intertar cargar el config.");
    exit(EXIT_FAILURE);
}

PUERTO_ESCUCHA = config_get_string_value(memoria_config, "PUERTO_ESCUCHA");
TAM_MEMORIA = config_get_int_value(memoria_config, "TAM_MEMORIA");
TAM_PAGINA = config_get_int_value(memoria_config, "TAM_PAGINA");
PATH_INSTRUCCIONES = config_get_string_value(memoria_config, "PATH_INSTRUCCIONES");
RETARDO_RESPUESTA = config_get_int_value(memoria_config, "RETARDO_RESPUESTA");

return 0;
}