#ifndef TYPES_H_
#define TYPES_H_

#include <stdlib.h>
#include <stdint.h>

/* TIPO DE DATO */
typedef struct {
    uint32_t dni;
    uint8_t edad;
    uint32_t pasaporte;
    uint32_t nombre_length;
    char* nombre;
} t_persona;

#endif /* TYPES_H_ */