#include "filesystem_instructions.h"

bool create_file(char *nombre)
{
    create_fcb(nombre);
    log_info(logger_main, "Crear archivo: %s", nombre);
    return true;
}

bool open_file(char *nombre)
{
    t_fcb *fcb = load_fcb(nombre);
    if (fcb == NULL)
    {
        log_info(logger_main, "Abrir archivo: %s - El archivo no existe", nombre);
        return false;
    }
    log_info(logger_main, "Abrir archivo: %s", fcb->nombre_archivo);
    free(fcb->nombre_archivo);
    free(fcb);
    return true;
}

bool is_greater_than_max_size_file_allowed(int tamanio_archivo)
{
    return tamanio_archivo > superbloque->block_size * (1 + get_cpb());
}

bool truncate_file(char *nombre, int nuevo_tamanio)
{
    t_fcb *fcb = load_fcb(nombre);
    if (fcb == NULL)
        return false;
    if (is_greater_than_max_size_file_allowed(nuevo_tamanio))
    {
        log_error(logger_main, "No se puede truncar el archivo %s porque supera el tamaño maximo permitido", nombre);
        return false;
    }
    if (nuevo_tamanio > fcb->tamanio_archivo)
    {
        uint32_t bloques_Adicionales = calculate_additionalBlocks(fcb, nuevo_tamanio);
        if (bloques_Adicionales > 0)
        {
            if (fcb->tamanio_archivo == 0)
            {
                int bloque_libre = obtener_bloque_libre();
                fcb->puntero_directo = bloque_libre;
                bloques_Adicionales--;
            }
            while (bloques_Adicionales > 0)
            {
                if (fcb->puntero_indirecto == 0)
                {
                    fcb->puntero_indirecto = obtener_bloque_libre();
                    uint32_t *punterosIndirectos = malloc(sizeof(uint32_t) * get_cpb());
                    for (int i = 0; i < get_cpb(); i++)
                        punterosIndirectos[i] = 0;
                    punterosIndirectos[0] = obtener_bloque_libre();
                    sleep(config_fs->retardo_acceso_bloque / 1000);
                    memcpy(blocks + fcb->puntero_indirecto * superbloque->block_size, punterosIndirectos, sizeof(uint32_t) * get_cpb());
                    free(punterosIndirectos);
                }
                else
                {
                    uint32_t *punterosIndirectos = obtenerPunterosIndirectos(fcb);
                    uint32_t cantPunterosIndirectos = cantidadPunterosIndirectos(punterosIndirectos);
                    punterosIndirectos[cantPunterosIndirectos] = obtener_bloque_libre();
                    sleep(config_fs->retardo_acceso_bloque / 1000);
                    memcpy(blocks + fcb->puntero_indirecto * superbloque->block_size, punterosIndirectos, sizeof(uint32_t) * get_cpb());
                    free(punterosIndirectos);
                }
                bloques_Adicionales--;
            }
        }
    }
    else if (nuevo_tamanio < fcb->tamanio_archivo)
    {
        if (nuevo_tamanio == 0)
        {
            set_freeBlock_bitmap(fcb->puntero_directo);
            fcb->puntero_directo = 0;
            uint32_t *punterosIndirectos = obtenerPunterosIndirectos(fcb);
            if (punterosIndirectos != NULL)
            {
                int cantPunterosIndirectos = cantidadPunterosIndirectos(punterosIndirectos);
                for (int i = 0; i < cantPunterosIndirectos; i++)
                    set_freeBlock_bitmap(punterosIndirectos[i]);
                set_freeBlock_bitmap(fcb->puntero_indirecto);
                fcb->puntero_indirecto = 0;
            }
        }
        else
        {
            int bloques_a_liberar = calculate_freeBlocks(fcb, nuevo_tamanio);
            uint32_t *punterosIndirectos = obtenerPunterosIndirectos(fcb);
            int cantPunterosIndirectos = cantidadPunterosIndirectos(punterosIndirectos);
            if (cantPunterosIndirectos == bloques_a_liberar && punterosIndirectos != NULL)
            {
                for (int i = 0; i < cantPunterosIndirectos; i++)
                    set_freeBlock_bitmap(punterosIndirectos[i]);
                set_freeBlock_bitmap(fcb->puntero_indirecto);
                fcb->puntero_indirecto = 0;
                free(punterosIndirectos);
            }
            else if (punterosIndirectos != NULL)
            {
                int i = cantPunterosIndirectos - 1;
                while (bloques_a_liberar > 0)
                {
                    set_freeBlock_bitmap(punterosIndirectos[i]);
                    punterosIndirectos[i] = 0;
                    bloques_a_liberar--;
                    i--;
                }
                sleep(config_fs->retardo_acceso_bloque / 1000);
                memcpy(blocks + fcb->puntero_indirecto * superbloque->block_size, punterosIndirectos, sizeof(uint32_t) * get_cpb());
                free(punterosIndirectos);
            }
        }
    }
    fcb->tamanio_archivo = nuevo_tamanio;
    update_fcb(fcb);
    log_info(logger_main, "Truncar archivo: %s - Tamaño: %d ", fcb->nombre_archivo, fcb->tamanio_archivo);
    free(fcb->nombre_archivo);
    free(fcb);
    return true;
}

bool read_file(char *nombre, int puntero_archivo, int cant_bytes, int direccion_fisica)
{
    int cant_bytes_inicial = cant_bytes;
    bool status = true;
    t_fcb *fcb = load_fcb(nombre);
    if (fcb == NULL)
        return false;
    if (fcb->tamanio_archivo < puntero_archivo + cant_bytes)
    {
        log_error(logger_main, "No se puede leer el archivo %s porque el tamaño a leer superan el tamaño del archivo", nombre);
        free(fcb->nombre_archivo);
        free(fcb);
        return false;
    }

    void *stream = malloc(cant_bytes);

    int numero_de_bloque_inicial = puntero_archivo / superbloque->block_size;
    int cantidad_de_bloques_a_leer = cant_bytes / superbloque->block_size;
    int offset_aux = cant_bytes - cantidad_de_bloques_a_leer * superbloque->block_size;
    int offset_dentro_bloque_pArchivo = puntero_archivo - (puntero_archivo / superbloque->block_size) * superbloque->block_size;
    int cant_bytes_disponibles = superbloque->block_size - offset_dentro_bloque_pArchivo;

    if (offset_aux > cant_bytes_disponibles)
        cantidad_de_bloques_a_leer++;

    if (cant_bytes % superbloque->block_size > 0)
        cantidad_de_bloques_a_leer++;

    uint32_t *punterosIndirectos = NULL;
    int cant_bytes_a_leer = 0;

    if (fcb->puntero_indirecto != 0)
        punterosIndirectos = obtenerPunterosIndirectos(fcb);

    if (numero_de_bloque_inicial == 0)
    {
        if (cantidad_de_bloques_a_leer == 1)
        {
            sleep(config_fs->retardo_acceso_bloque / 1000);
            memcpy(stream, blocks + fcb->puntero_directo * superbloque->block_size + offset_dentro_bloque_pArchivo, cant_bytes);
            log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, 0, fcb->puntero_directo);
        }
        else
        {
            sleep(config_fs->retardo_acceso_bloque / 1000);
            memcpy(stream, blocks + fcb->puntero_directo * superbloque->block_size + offset_dentro_bloque_pArchivo, cant_bytes_disponibles);
            log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, 0, fcb->puntero_directo);
            cant_bytes = cant_bytes - cant_bytes_disponibles;
            cant_bytes_a_leer += cant_bytes_disponibles;
            for (int i = 0; i < cantidad_de_bloques_a_leer - 1; i++)
            {
                int offset_bloque_pi = punterosIndirectos[i] * superbloque->block_size;
                if (superbloque->block_size < cant_bytes)
                {
                    sleep(config_fs->retardo_acceso_bloque / 1000);
                    memcpy(stream + cant_bytes_a_leer, blocks + offset_bloque_pi, superbloque->block_size);
                    log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                    cant_bytes = cant_bytes - superbloque->block_size;
                    cant_bytes_a_leer = cant_bytes_a_leer + superbloque->block_size;
                }
                else
                {
                    sleep(config_fs->retardo_acceso_bloque / 1000);
                    memcpy(stream + cant_bytes_a_leer, blocks + offset_bloque_pi, cant_bytes);
                    log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                }
            }
        }
    }
    else
    {
        int offset_bloque_pArchivo_2 = offset_dentro_bloque_pArchivo;
        numero_de_bloque_inicial -= 1;
        for (int i = numero_de_bloque_inicial; i < numero_de_bloque_inicial + cantidad_de_bloques_a_leer; i++)
        {
            int offset_bloque_pi = punterosIndirectos[i] * superbloque->block_size;
            if (superbloque->block_size < cant_bytes)
            {
                sleep(config_fs->retardo_acceso_bloque / 1000);
                memcpy(stream + cant_bytes_a_leer, blocks + offset_bloque_pi + offset_dentro_bloque_pArchivo, superbloque->block_size - offset_bloque_pArchivo_2);
                log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                offset_dentro_bloque_pArchivo = 0;
                cant_bytes_a_leer = cant_bytes_a_leer + (superbloque->block_size - offset_bloque_pArchivo_2);
                offset_bloque_pArchivo_2 = 0;
                cant_bytes = cant_bytes - cant_bytes_a_leer;
            }
            else
            {
                sleep(config_fs->retardo_acceso_bloque / 1000);
                memcpy(stream + cant_bytes_a_leer, blocks + offset_bloque_pi + offset_dentro_bloque_pArchivo, cant_bytes);
                log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
            }
        }
    }
    log_info(logger_main, "Leer archivo: %s - Puntero: %d - Memoria: %d - Tamanio: %d", fcb->nombre_archivo, puntero_archivo, direccion_fisica, cant_bytes_inicial);

    t_info_write *info_write = malloc(sizeof(t_info_write));
    info_write->info = malloc(sizeof(t_info));
    info_write->base_address = direccion_fisica;
    info_write->info->data = malloc(cant_bytes_inicial);
    memcpy(info_write->info->data, stream, cant_bytes_inicial);
    info_write->info->size = cant_bytes_inicial;

    bool res = send_info_write(memory_socket, info_write, logger_aux);
    if (!res)
    {
        log_error(logger_aux, "No se pudo enviar el info de F_READ a memoria");
        status = false;
    }
    else
    {
        t_package *package = get_package(memory_socket, logger_aux);
        t_status_code status_code;
        if (package->operation_code == STATUS_CODE)
        {
            status_code = get_status_code(package);
            if (status_code == SUCCESS)
                log_info(logger_aux, "Se pudo escribir en memoria en el F_READ");
            else
            {
                log_error(logger_aux, "No se pudo escribir en memoria en el F_READ");
                status = false;
            }
        }
        else
        {
            log_error(logger_aux, "No se pudo obtener el OK de memoria en el F_READ");
            status = false;
        }
        package_destroy(package);
    }
    info_write_destroy(info_write);
    free(stream);
    free(punterosIndirectos);
    free(fcb->nombre_archivo);
    free(fcb);
    return status;
}

bool write_file(char *nombre, int puntero_archivo, int cant_bytes, int direccion_fisica)
{
    t_fcb *fcb = load_fcb(nombre);
    if (fcb == NULL)
        return false;

    if (fcb->tamanio_archivo < puntero_archivo + cant_bytes)
    {
        log_error(logger_main, "No se puede escribir en el archivo %s porque el tamaño a escribir superan el tamaño del archivo", nombre);
        free(fcb->nombre_archivo);
        free(fcb);
        return false;
    }
    // Solicito los datos a memoria
    t_info_read *info_read = malloc(sizeof(t_info_read));
    info_read->base_address = direccion_fisica;
    info_read->size = cant_bytes;
    send_info_read(memory_socket, info_read, logger_aux);
    free(info_read);
    t_package *package = get_package(memory_socket, logger_aux);

    int cant_bytes_inicial = cant_bytes;

    switch (package->operation_code)
    {
    case INFO:
        t_info *info = get_info(package);

        int numero_de_bloque_inicial = puntero_archivo / superbloque->block_size;
        int cantida_de_bloques_a_escribir = cant_bytes / superbloque->block_size;
        int offset_aux = cant_bytes - cantida_de_bloques_a_escribir * superbloque->block_size;
        int offset_dentro_bloque_pArchivo = puntero_archivo - (puntero_archivo / superbloque->block_size) * superbloque->block_size;
        int cant_bytes_disponibles = superbloque->block_size - offset_dentro_bloque_pArchivo;

        if (offset_aux > cant_bytes_disponibles)
            cantida_de_bloques_a_escribir++;
        if (cant_bytes % superbloque->block_size > 0)
            cantida_de_bloques_a_escribir++;

        uint32_t *punterosIndirectos = NULL;
        if (fcb->puntero_indirecto != 0)
            punterosIndirectos = obtenerPunterosIndirectos(fcb);

        int cant_bytes_escritos = 0;

        if (numero_de_bloque_inicial == 0)
        {
            if (cantida_de_bloques_a_escribir == 1)
            {
                uint32_t offset = fcb->puntero_directo * superbloque->block_size + offset_dentro_bloque_pArchivo;
                sleep(config_fs->retardo_acceso_bloque / 1000);
                memcpy(blocks + offset, info->data, cant_bytes);
                log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, 0, fcb->puntero_directo);
            }
            else
            {
                sleep(config_fs->retardo_acceso_bloque / 1000);
                memcpy(blocks + fcb->puntero_directo * superbloque->block_size + offset_dentro_bloque_pArchivo, info->data, cant_bytes_disponibles);
                log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, 0, fcb->puntero_directo);
                cant_bytes = cant_bytes - cant_bytes_disponibles;
                cant_bytes_escritos += cant_bytes_disponibles;
                for (int i = 0; i < cantida_de_bloques_a_escribir - 1; i++)
                {
                    int offset_bloque_pi = punterosIndirectos[i] * superbloque->block_size;
                    if (superbloque->block_size < cant_bytes)
                    { // escribis en todo el bloque y falta mas
                        sleep(config_fs->retardo_acceso_bloque / 1000);
                        memcpy(blocks + offset_bloque_pi, info->data + cant_bytes_escritos, superbloque->block_size);
                        log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                        cant_bytes = cant_bytes - superbloque->block_size;
                        cant_bytes_escritos = cant_bytes_escritos + superbloque->block_size;
                    }
                    else
                    {
                        sleep(config_fs->retardo_acceso_bloque / 1000);
                        memcpy(blocks + offset_bloque_pi, info->data + cant_bytes_escritos, cant_bytes);
                        log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                    }
                }
            }
        }
        else
        {
            int offset_bloque_pArchivo_2 = offset_dentro_bloque_pArchivo;
            numero_de_bloque_inicial -= 1;
            for (int i = numero_de_bloque_inicial; i < numero_de_bloque_inicial + cantida_de_bloques_a_escribir; i++)
            {
                int offset_bloque_pi = punterosIndirectos[i] * superbloque->block_size;
                if (superbloque->block_size < cant_bytes)
                {
                    sleep(config_fs->retardo_acceso_bloque / 1000);
                    memcpy(blocks + offset_bloque_pi + offset_dentro_bloque_pArchivo, info->data + cant_bytes_escritos, superbloque->block_size - offset_bloque_pArchivo_2);
                    log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                    cant_bytes_disponibles = 0;
                    offset_dentro_bloque_pArchivo = 0;
                    cant_bytes_escritos = cant_bytes_escritos + (superbloque->block_size - offset_bloque_pArchivo_2);
                    offset_bloque_pArchivo_2 = 0;
                    cant_bytes = cant_bytes - cant_bytes_escritos;
                }
                else
                {
                    sleep(config_fs->retardo_acceso_bloque / 1000);
                    memcpy(blocks + offset_bloque_pi + offset_dentro_bloque_pArchivo, info->data + cant_bytes_escritos, cant_bytes);
                    log_info(logger_main, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", nombre, i + 1, punterosIndirectos[i]);
                }
            }
        }

        log_info(logger_main, "Escribir archivo: %s - Puntero: %d - Memoria: %d - Tamanio: %d", fcb->nombre_archivo, puntero_archivo, direccion_fisica, cant_bytes_inicial);
        info_destroy(info);
        free(punterosIndirectos);
        break;
    default:
        break;
    }

    free(fcb->nombre_archivo);
    free(fcb);
    package_destroy(package);
    return true;
}
