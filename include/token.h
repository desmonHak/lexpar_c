#ifndef LEXPAR_TOKEN_H
#define LEXPAR_TOKEN_H

#include <stdint.h>

#include "time_code.h"

#include "global.h"

#ifndef Token_id
typedef uint16_t Token_id;
#define Token_id Token_id
#endif

/*
permite especificar los tipos de tockens que se pueden procesar
*/
#ifndef Token_t
typedef struct Token_t
{
    const char*     name_token; // nombre del token
    Object               value; // tipo de valor que se usa para identificar
    Token_id              type; // tipo de token (id numerico unico)
} Token_t;
#define Token_t Token_t
#endif

/*
permite construir un token con un valor.
*/
typedef struct Token_build_t {
    Token_t*            token; // token que identifica este valor
    Object      value_process; // valor obtenido en el analisis
} Token_build_t;

typedef const Token_id (*func_auto_increment)(void);


#ifndef none_name
#define none_name NULL
#endif

#ifndef create_name_token
#define create_name_token(name) #name
#endif

#ifndef build_token_special
#define build_token_special(name) "?{" #name "}"
#endif

#ifndef create_token
#define create_token(name, value, type) _Generic((type),              \
    Token_id:               create_token_with_number,           \
    func_auto_increment:    create_token_with_autoincrement,    \
    default: create_token_with_number)(name, value, type)
#endif

Token_t* create_token_with_number(const char* name_token, Object value, Token_id type);
Token_build_t* init_token_build(Object value_process);
Token_t* create_token_with_autoincrement(
    const char* name_token,
    Object value,
    func_auto_increment auto_increment
);
void print_token(Token_t *token);
#endif