/*#include "parser.h"


bool el_proceso_es_valido(FILE* instrucciones){

    char* linea = malloc(16);
    char** instruccion_leida;
    ID_INSTRUCCION id = EXIT;
  
        if(!es_una_instruccion_valida(instruccion_leida)){
            return false;
        }
        id = get_id(instruccion_leida[0]);
        string_array_destroy(instruccion_leida);
    
 
    free(linea);
    return true;
}

bool es_una_instruccion_valida(char** instruccion){
    if(string_contains(IDENTIFICADORES_VALIDOS, instruccion[0])){   //podria hacerla en una funcion aparte, VER DESPUES
        if(tiene_los_parametros_correctos(instruccion)){
            return true;
        }
        return false;
    }
    printf("El identificador %s es desconocido", instruccion[0]);

    return false;
}

bool es_valido(char* identificador){
    return string_contains(IDENTIFICADORES_VALIDOS, identificador);
}

bool tiene_los_parametros_correctos(char** instruccion){

    int cantidad_de_parametros = string_array_size(instruccion);
    int cant_parametros_leidos = cant_parametros_leidos + cantidad_de_parametros; //puede tener errores si no esta inicializada en 0, PROBAR

    if(cantidad_de_parametros == parametros_segun_id(instruccion[0])){
        return true;
    }
    else 
        return false;
}

//devuelve la cantidad de parametros del identificador ingresado
int parametros_segun_id(char* identificador){

    ID_INSTRUCCION id = get_id(identificador);
    int cantidad_de_parametros_del_identificador_ingresado = cantidad_de_parametros(id);

    return cantidad_de_parametros_del_identificador_ingresado;
}

//devuelve la cantidad de parametros que tiene que tener cada instruccion
int cantidad_de_parametros(ID_INSTRUCCION identificador){

    switch (identificador){
    case F_READ:
    case F_WRITE:
        return 3;
        break;
    case SET:
    case MOV_IN:
    case MOV_OUT:
    case F_TRUNCATE:
    case F_SEEK:
    case CREATE_SEGMENT:
        return 2;
        break;
    case EXIT:
    case YIELD:
        return 0;
        break;
    }
    return 0;
}

//devuelve la instruccion si es que es valida
ID_INSTRUCCION get_id(char* identificador){
    
    ID_INSTRUCCION id;
    char** identificadores = string_split(IDENTIFICADORES_VALIDOS, ","); //Separo un string con un separador, retorna un array con cada palabra

    for(int i = 0; i < string_array_size(identificadores); i++) { //Retorna la cantidad de líneas del array de strings
        if(string_equals_ignore_case(identificadores[i], identificador)){ //retorna si dos string son iguales
            id = i;
        }
    }
    string_array_destroy(identificadores);
    return id;
}

*/