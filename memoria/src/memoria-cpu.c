#include "../include/memoria-cpu.h"

void conexion_memoria_cpu() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_cpu);
		switch (cod_op) {
		case MENSAJE:
			log_info(memoria_logger, "Mensaje recibido con el numero");
			break;
		case PAQUETE:
			break;
		case RECIBIR_PC_PID:
			uint8_t pid;
			uint32_t pc;
			if(!recv_pc_pid(fd_cpu, &pc, &pid)) { 
				log_error(memoria_logger, "Hubo un error al recibir el PC de un proceso");
			}

			t_list* instrucciones = list_get(instrucciones_por_proceso, pid);

			if (instrucciones == NULL) {
				log_warning(memoria_logger, "La lista de instrucciones se encuentra vacia");
			}
			
			usleep(RETARDO_RESPUESTA);
			
			if (pc <= list_size(instrucciones)) {
				char* instruccion = list_get(instrucciones, pc);
				send_instruccion(fd_cpu, instruccion, strlen(instruccion) + 1);
				log_info(memoria_logger, "Instruccion %d enviada", pc);
			}
			break;
		case RECIBIR_TAMANIO:
			uint8_t pid_resize = 0; // Pongo 0 por ahora
			uint32_t tamanio;
			if(!recv_tamanio(fd_cpu, &tamanio)) { // ? Falta q reciba el pid para las tablas de paginas de todos los procesos
				log_error(memoria_logger, "Hubo un error al recibir el Resize");
			}

			// Crear tabla de paginas para cada proceso
			t_list* tabla_paginas = list_get(tabla_paginas_por_proceso, pid_resize);
            int paginas_necesarias = ceil(tamanio / TAM_PAGINA);

            if (tabla_paginas == NULL) {
                // Primera vez que se recibe una solicitud para este proceso
                tabla_paginas = list_create();
                for (int i = 0; i < paginas_necesarias; i++) {
                    int marco = obtener_marco_libre(marcos_ocupados);
                    if (marco != -1) {
                        list_add(tabla_paginas, (void*)(intptr_t)marco);
                        log_info(memoria_logger, "Marco %d asignado a la página %d", marco, i);
                    } else {
                        log_error(memoria_logger, "No hay marcos libres disponibles");
                        break;
                    }
                }
                list_add_in_index(tabla_paginas_por_proceso, pid, tabla_paginas);
            } else {
                // Ajustar la tabla de páginas existente
                int paginas_actuales = list_size(tabla_paginas);
                if (paginas_necesarias > paginas_actuales) {
                    // Aumentar tamaño --> añadir más marcos
                    for (int i = paginas_actuales; i < paginas_necesarias; i++) {
                        int marco = obtener_marco_libre(marcos_ocupados);
                        if (marco != -1) {
                            list_add(tabla_paginas, (void*)(intptr_t)marco);
                            log_info(memoria_logger, "Marco %d asignado a la página %d", marco, i);
                        } else {
                            log_error(memoria_logger, "No hay marcos libres disponibles");
                            break;
                        }
                    }
                } else if (paginas_necesarias < paginas_actuales) {
                    // Disminuir tamaño --> liberar marcos
                    for (int i = paginas_actuales - 1; i >= paginas_necesarias; i--) {
                        int marco = (intptr_t)list_remove(tabla_paginas, i);
                        liberar_marco(marcos_ocupados, marco);
                        log_info(memoria_logger, "Marco %d liberado de la página %d", marco, i);
                    }
                }
            }

			// Esto se hace cuando recibe una instruccion que necesite escribir en memoria de usuario --> va en otro lado
			int pagina_actual = 0;
            while (tamanio > 0) {
				// Conseguir el marco asignado a esa pagina para poder escribir
                t_list* tabla_paginas_actual = list_get(tabla_paginas_por_proceso, pid_resize);
                int marco_libre = obtener_marco_asignado(pid_resize, pagina_actual, tabla_paginas_actual);
                if (marco_libre == -1) {
                    // No hay espacio en la página actual, pasar a la siguiente página
                    pagina_actual++;
                    continue;
                }

                // Calcular el tamaño de datos a escribir en este marco
                uint32_t tamanio_a_escribir = min(tamanio, TAM_PAGINA);

                // Escribir los datos en el marco actual
                void* datos_a_escribir = "Prueba"; // Esto se cuando recibe una instruccion de CPU, por ahora es "Prueba"
                escribir_memoria(espacio_usuario, marco_libre, tamanio_a_escribir, datos_a_escribir);

                // Actualizar el tamaño restante y pasar al siguiente marco
                tamanio -= tamanio_a_escribir;
                pagina_actual++;
            }
			break;
		case -1:
			log_error(memoria_logger, "El CPU se desconecto. Terminando servidor");
			control = 0;
            break;
		default:
			log_warning(memoria_logger, "Operacion desconocida.");
			break;
		}
	}
}