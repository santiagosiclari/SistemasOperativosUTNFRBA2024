#include "../include/memoria-cpu.h"

void conexion_memoria_cpu() {
    bool control = 1;
	while (control) {
		int cod_op = recibir_operacion(fd_cpu);
		switch (cod_op) {
		case MENSAJE:
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
			uint8_t pid_resize;
			uint32_t tamanio;
			if(!recv_tamanio(fd_cpu, &tamanio, &pid_resize)) {
				log_error(memoria_logger, "Hubo un error al recibir el Resize");
			}

            usleep(RETARDO_RESPUESTA);

            int paginas_necesarias = ceil(tamanio / (float)TAM_PAGINA);

            if (!existe_tabla_paginas(pid_resize)) {
                // Primera vez que se recibe una solicitud para este proceso
                log_info(memoria_logger, "Creacion de Tabla de Paginas");
                log_info(memoria_logger, "PID: %d - Tamaño: %d", pid_resize, paginas_necesarias * TAM_PAGINA);

                // Verificar si hay suficientes marcos disponibles antes de crear la tabla de páginas
                int marcos_disponibles = contar_marcos_libres(marcos_ocupados);

                if (paginas_necesarias > marcos_disponibles) {
                    log_error(memoria_logger, "Out Of Memory: No hay suficientes marcos libres para el nuevo proceso");
                    send_out_of_memory(fd_cpu, pid_resize);
                    break;
                }

			    // Crear tabla de paginas para cada proceso
                t_list* tabla_paginas = list_create();
                for (int i = 0; i < paginas_necesarias; i++) {
                    uint32_t marco = obtener_marco_libre(marcos_ocupados);
                    if (marco != -1) {
                        uint32_t* marco_ptr = malloc(sizeof(uint32_t));
                        *marco_ptr = (uint32_t)marco;
                        list_add(tabla_paginas, marco_ptr);
                        log_info(memoria_logger, "Marco %d asignado a la página %d", marco, i);
                    } else {
                        log_error(memoria_logger, "No hay marcos libres disponibles");
                        break;
                    }
                }
                list_add_in_index(tabla_paginas_por_proceso, pid_resize, tabla_paginas);
            } else {
                // Ajustar la tabla de páginas existente
			    t_list* tabla_paginas = list_get(tabla_paginas_por_proceso, pid_resize);
                int paginas_actuales = list_size(tabla_paginas);
                if (paginas_necesarias > paginas_actuales) {
                    // Verificar si hay suficientes marcos disponibles antes de ampliar
                    int marcos_disponibles = contar_marcos_libres(marcos_ocupados);
                    int marcos_necesarios = paginas_necesarias - paginas_actuales;

                    if (marcos_necesarios > marcos_disponibles) {
                        log_error(memoria_logger, "Out Of Memory: No hay suficientes marcos libres para ampliar el proceso");
                        send_out_of_memory(fd_cpu, pid_resize);
                        break;
                    }

                    // Aumentar tamaño --> añadir más marcos
                    log_info(memoria_logger, "Ampliacion del Proceso");
                    log_info(memoria_logger, "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d", pid_resize, paginas_actuales * TAM_PAGINA, tamanio);
                    for (int i = paginas_actuales; i < paginas_necesarias; i++) {
                        uint32_t marco = obtener_marco_libre(marcos_ocupados);
                        if (marco != -1) {
                            uint32_t* marco_ptr = malloc(sizeof(uint32_t));
                            *marco_ptr = (uint32_t)marco;
                            list_add(tabla_paginas, marco_ptr);
                            log_info(memoria_logger, "Marco %d asignado a la página %d", marco, i);
                        } else {
                            log_error(memoria_logger, "No hay marcos libres disponibles");
                            break;
                        }
                    }
                } else if (paginas_necesarias < paginas_actuales) {
                    // Disminuir tamaño --> liberar marcos
                    log_info(memoria_logger, "Reduccion del Proceso");
                    log_info(memoria_logger, "PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d", pid_resize, paginas_actuales * TAM_PAGINA, tamanio);
                    for (int i = paginas_actuales - 1; i >= paginas_necesarias; i--) {
                        int marco = (intptr_t)list_remove(tabla_paginas, i);
                        liberar_marco(marcos_ocupados, marco);
                        log_info(memoria_logger, "Marco %d liberado de la página %d", marco, i);
                    }
                }
            }
            break;
        case ESCRIBIR_MEMORIA:
            uint8_t pid_a_escribir;
            uint32_t direccion_fisica_escribir, tamanio_a_escribir;
            void* datos_escribir;
            if (!recv_escribir_memoria(fd_cpu, &pid_a_escribir, &direccion_fisica_escribir, &datos_escribir, &tamanio_a_escribir)) {
                log_error(memoria_logger, "Hubo un error al recibir la instruccion de escribir memoria.");
                break;
            }

            log_info(memoria_logger, "MOV_OUT: Direccion fisica: %d, Tamaño: %d", direccion_fisica_escribir, tamanio_a_escribir);

            usleep(RETARDO_RESPUESTA);
            log_info(memoria_logger, "Acceso a espacio de usuario: PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamaño: %d", pid_a_escribir, direccion_fisica_escribir, tamanio_a_escribir);

            int pagina_actual_escribir = floor(direccion_fisica_escribir / TAM_PAGINA);
            uint32_t desplazamiento_actual_escribir = direccion_fisica_escribir - pagina_actual_escribir * TAM_PAGINA; // Se utiliza para llevar un seguimiento del desplazamiento dentro de la página actual
            while (tamanio_a_escribir > 0) {
                // Conseguir el marco asignado a esa página para poder escribir
                t_list* tabla_paginas_actual_escribir = list_get(tabla_paginas_por_proceso, pid_a_escribir);
                uint32_t marco_libre_escribir = obtener_marco_asignado(pid_a_escribir, pagina_actual_escribir, tabla_paginas_actual_escribir);
                if (marco_libre_escribir == -1) {
                    // No hay espacio en la página actual, pasar a la siguiente página
                    pagina_actual_escribir++;
                    desplazamiento_actual_escribir = 0; // Reiniciar el desplazamiento para la nueva página
                    continue;
                }

                // Calcular el tamaño de datos a escribir en este marco
                uint32_t bytes_disponibles_pagina = TAM_PAGINA - desplazamiento_actual_escribir;
                uint32_t tamanio_a_escribir_actual = min(tamanio_a_escribir, bytes_disponibles_pagina);

                // Escribir los datos en el marco actual
                if (!escribir_memoria(espacio_usuario, marco_libre_escribir, desplazamiento_actual_escribir, tamanio_a_escribir_actual, datos_escribir)) {
                    log_error(memoria_logger, "Error al escribir en la memoria.");
                    // Error en escritura --> envia -1
                    send_escritura_ok(fd_cpu, -1);
                    break;
                }

                // Actualizar el tamaño restante y el desplazamiento dentro de la página actual
                tamanio_a_escribir -= tamanio_a_escribir_actual;
                desplazamiento_actual_escribir += tamanio_a_escribir_actual;

                // Si hay más datos por escribir, pasar a la siguiente página
                if (tamanio_a_escribir > 0) {
                    pagina_actual_escribir++;
                    desplazamiento_actual_escribir = 0; // Reiniciar el desplazamiento para la nueva página
                }
            }
            // Escritura OK --> envia 1
            send_escritura_ok(fd_cpu, 1);
            free(datos_escribir);
            break;
        case LEER_MEMORIA:
            uint8_t pid_a_leer;
            uint32_t direccion_fisica_leer, tamanio_a_leer;
            if (!recv_leer_memoria(fd_cpu, &pid_a_leer, &direccion_fisica_leer, &tamanio_a_leer)) {
                log_error(memoria_logger, "Hubo un error al recibir la instruccion de leer memoria.");
                break;
            }

            usleep(RETARDO_RESPUESTA);
            log_info(memoria_logger, "Acceso a espacio de usuario: PID: %d - Accion: LEER - Direccion fisica: %u - Tamaño: %u", pid_a_leer, direccion_fisica_leer, tamanio_a_leer);

            int pagina_actual_leer = floor(direccion_fisica_leer / TAM_PAGINA);
            uint32_t desplazamiento_actual_leer = direccion_fisica_leer % TAM_PAGINA; // Ajuste de cálculo del desplazamiento dentro de la página

            t_list* tabla_paginas_actual = list_get(tabla_paginas_por_proceso, pid_a_leer);
            int marco_asignado_leer = obtener_marco_asignado(pid_a_leer, pagina_actual_leer, tabla_paginas_actual);
            if (marco_asignado_leer == -1) {
                log_error(memoria_logger, "No se encontró el marco asignado para la página %d del proceso %d.", pagina_actual_leer, pid_a_leer);
                break;
            }

            void* datos_leer = malloc(tamanio_a_leer);

            if (!leer_memoria(espacio_usuario, marco_asignado_leer, desplazamiento_actual_leer, tamanio_a_leer, datos_leer)) {
                log_error(memoria_logger, "Error al leer la memoria.");
                free(datos_leer);
                break;
            }

            send_valor_memoria(fd_cpu, datos_leer, tamanio_a_leer);

            free(datos_leer);
            break;
        case NUMERO_PAGINA:
            uint8_t pid_pagina;
			uint32_t numero_pagina, desplazamiento;
			if (!recv_num_pagina(fd_cpu, &pid_pagina, &numero_pagina, &desplazamiento)) {
				log_error(memoria_logger, "Hubo un error al recibir el numero de pagina");
				break;
			}

            usleep(RETARDO_RESPUESTA);

			t_list* tabla_paginas_marco = list_get(tabla_paginas_por_proceso, pid_resize);
            uint32_t* marco_ptr = (uint32_t*)list_get(tabla_paginas_marco, numero_pagina);
            uint32_t num_marco = *marco_ptr;

            send_num_marco(fd_cpu, pid_pagina, numero_pagina, num_marco);
            log_info(memoria_logger, "Numero de marco enviado a CPU: %d", num_marco);

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