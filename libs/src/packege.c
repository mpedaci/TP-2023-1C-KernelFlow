#include "package.h"

/* BUFFERS TIPOS DE DATOS -> SEND */

t_buffer* t_persona_create_buffer(t_persona persona)
{
    t_buffer* buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t) * 3         // DNI, Pasaporte y longitud del nombre
                 + sizeof(uint8_t)              // Edad
                 + strlen(persona.nombre) + 1;  // La longitud del string nombre. Le sumamos 1 para enviar tambien el caracter centinela '\0'. Esto se podría obviar, pero entonces deberíamos agregar el centinela en el receptor.

    void* stream = malloc(buffer->size);
    // Desplazamiento
    int offset = 0;

    // ACA SE DEFINE EL STRUCT Y SUS CAMPOS A GUARDAR
    // TENER EN CUENTA LOS TIPOS DE DATOS
    memcpy(stream + offset, &persona.dni, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &persona.edad, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &persona.pasaporte, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    // Para el nombre primero mandamos el tamaño y luego el texto en sí:
    memcpy(stream + offset, &persona.nombre_length, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, persona.nombre, strlen(persona.nombre) + 1);

    // No tiene sentido seguir calculando el desplazamiento, ya ocupamos el buffer completo
    buffer->stream = stream;
    return buffer;
}

t_buffer* t_pcontexto_create_buffer(t_pcontexto* pcontexto) {
    t_buffer* buffer = malloc(sizeof(t_buffer));
    t_buffer* buffer_instrucciones = create_buffer_instructions(pcontexto->instructions);

    buffer->size = sizeof(uint32_t) * 2 
                 + buffer_instrucciones->size 
                 + sizeof(pcontexto->registers->AX) * 4
                 + sizeof(pcontexto->registers->EAX) * 4
                 + sizeof(pcontexto->registers->RAX) * 4;

    void* stream = malloc(sizeof(buffer->size));

    int offset = 0;

    memcpy(stream, pcontexto->pib, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, buffer_instrucciones->stream, buffer_instrucciones->size);
    offset += buffer_instrucciones->size;
    memcpy(stream + offset, pcontexto->program_counter, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &(pcontexto->registers->AX), sizeof(4));
    offset += sizeof(4);
    memcpy(stream + offset, &(pcontexto->registers->BX), sizeof(4));
    offset += sizeof(4);
    memcpy(stream + offset, &(pcontexto->registers->CX), sizeof(4));
    offset += sizeof(4);
    memcpy(stream + offset, &(pcontexto->registers->DX), sizeof(4));
    offset += sizeof(4);

    memcpy(stream + offset, &(pcontexto->registers->EAX), sizeof(8));
    offset += sizeof(8);
    memcpy(stream + offset, &(pcontexto->registers->EBX), sizeof(8));
    offset += sizeof(8);
    memcpy(stream + offset, &(pcontexto->registers->ECX), sizeof(8));
    offset += sizeof(8);
    memcpy(stream + offset, &(pcontexto->registers->EDX), sizeof(8));
    offset += sizeof(8);

    memcpy(stream + offset, &(pcontexto->registers->RAX), sizeof(16));
    offset += sizeof(16);
    memcpy(stream + offset, &(pcontexto->registers->RBX), sizeof(16));
    offset += sizeof(16);
    memcpy(stream + offset, &(pcontexto->registers->RCX), sizeof(16));
    offset += sizeof(16);
    memcpy(stream + offset, &(pcontexto->registers->RDX), sizeof(16));

    free(buffer_instrucciones);
    buffer->stream = stream;
    return buffer;
}

t_buffer* null_buffer(){
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = 0;
    buffer->stream = NULL;
    return buffer;
}

/* BUFFERS TIPOS DE DATOS -> RECV */

t_persona* t_persona_create_from_buffer(t_buffer* buffer)
{
    t_persona* persona = malloc(sizeof(t_persona));
    void* stream = buffer->stream;

    // ACA SE DEFINE EL STRUCT Y SUS CAMPOS A GUARDAR
    // TENER EN CUENTA LOS TIPOS DE DATOS
    // Deserializamos los campos que tenemos en el buffer
    memcpy(&(persona->dni), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(persona->edad), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    memcpy(&(persona->pasaporte), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    // Por último, para obtener el nombre, primero recibimos el tamaño y luego el texto en sí:
    memcpy(&(persona->nombre_length), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    persona->nombre = malloc(persona->nombre_length);
    memcpy(persona->nombre, stream, persona->nombre_length);

    return persona;
}

t_pcontexto* t_pcontexto_create_from_buffer(t_buffer* buffer) {
    t_pcontexto* pcontexto = malloc(sizeof(t_pcontexto));
    void* stream = buffer->stream;

    memcpy(&(pcontexto->pib), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    // usar intrucciones_create_from_buffer
    // sumarle al stream la cantidad de bytes que ocupen las instrucciones

    memcpy(&(pcontexto->program_counter), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    
    memcpy(&(pcontexto->registers->AX), stream, sizeof(4));
    stream += sizeof(4);
    memcpy(&(pcontexto->registers->BX), stream, sizeof(4));
    stream += sizeof(4);
    memcpy(&(pcontexto->registers->CX), stream, sizeof(4));
    stream += sizeof(4);
    memcpy(&(pcontexto->registers->DX), stream, sizeof(4));
    stream += sizeof(4);

    memcpy(&(pcontexto->registers->EAX), stream, sizeof(8));
    stream += sizeof(8);
    memcpy(&(pcontexto->registers->EBX), stream, sizeof(8));
    stream += sizeof(8);
    memcpy(&(pcontexto->registers->ECX), stream, sizeof(8));
    stream += sizeof(8);
    memcpy(&(pcontexto->registers->EDX), stream, sizeof(8));
    stream += sizeof(8);

    memcpy(&(pcontexto->registers->RAX), stream, sizeof(16));
    stream += sizeof(16);
    memcpy(&(pcontexto->registers->RBX), stream, sizeof(16));
    stream += sizeof(16);
    memcpy(&(pcontexto->registers->RCX), stream, sizeof(16));
    stream += sizeof(16);
    memcpy(&(pcontexto->registers->RDX), stream, sizeof(16));

    free(buffer);
    return pcontexto;
}

/* PAQUETES */

t_package* package_create(t_buffer *buffer, int operation_code)
{
    t_package* paquete = malloc(sizeof(t_package));
    paquete->operation_code = operation_code;
    if (buffer == NULL)
        paquete->buffer = null_buffer();
    else
        paquete->buffer = buffer;
    return paquete;
}

uint32_t package_get_size(t_package* paquete)
{
    return paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t);
}

bool package_send(int socket, t_package* paquete, t_log* logger)
{
    // Armamos el stream a enviar
    // Buffer Size + Size of Operation Code Var + Size of Buffer Size Var
    void* a_enviar = malloc(paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
    int offset = 0;
    memcpy(a_enviar + offset, &(paquete->operation_code), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
    if (send(socket, a_enviar, paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0) == -1){
        log_error(logger, "Error al enviar el paquete");
        free(a_enviar);
        return false;
    }
    printf("Enviado paquete con codigo de operacion: %d\n", paquete->operation_code);
    free(a_enviar);
    return true;
}

t_package* package_recv(int socket, t_log* logger){
    t_package* paquete = malloc(sizeof(t_package));
    paquete->buffer = malloc(sizeof(t_buffer));

    // Primero recibimos el codigo de operacion
    recv(socket, &(paquete->operation_code), sizeof(uint8_t), 0);
    log_info(logger, "Recibido codigo de operacion: %d", paquete->operation_code);
    // Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
    recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);
    
    return paquete;
}

void package_destroy(t_package* paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}