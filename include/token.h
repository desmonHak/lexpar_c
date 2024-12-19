#ifndef LEXPAR_TOKEN_H
#define LEXPAR_TOKEN_H

#include <stdint.h>

#include "../lib/structs_data_C/DebugLibC/time_code.h"
#include "global.h"

#ifndef Token_id
typedef uint16_t Token_id;
#define Token_id Token_id
#endif

#ifndef Token_t
typedef struct Token_t
{
    const char*     name_token;
    Object               value;
    Token_id              type;
} Token_t;
#define Token_t Token_t
#endif

typedef const Token_id (*func_auto_increment)(void);


#ifndef none_name
#define none_name NULL
#endif

#ifndef create_name_token
#define create_name_token(name) #name
#endif

#ifndef create_token
#define create_token(name, value, type) _Generic((type),              \
    Token_id:               create_token_with_number,           \
    func_auto_increment:    create_token_with_autoincrement,    \
    default: create_token_with_number)(name, value, type)
#endif

#include "../src/token.c"
#endif