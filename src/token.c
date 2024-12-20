#ifndef LEXPAR_TOKEN_C
#define LEXPAR_TOKEN_C

#include "../include/token.h"

Token_t* create_token_with_number(const char* name_token, Object value, Token_id type) {
    Token_t *token;
    debug_malloc(Token_t, token, sizeof(Token_t));

    token->value        = value;
    token->type         = type;
    token->name_token   = name_token;

    return token;
}

Token_build_t* init_token_build(Object value_process){
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , init_token_build)
            TYPE_DATA_DBG(Object, "value_process = %p")
        END_TYPE_FUNC_DBG,
        value_process);
    Token_build_t *token_b;
    debug_malloc(Token_build_t, token_b, sizeof(Token_build_t));

    token_b->token          = NULL;
    token_b->value_process  = value_process;

    return token_b;
}

Token_t* create_token_with_autoincrement(
        const char* name_token,
        Object value,
        func_auto_increment auto_increment
    ) {

    Token_t *token;
    debug_malloc(Token_t, token, sizeof(Token_t));

    token->name_token = name_token;
    token->value      = value;
    token->type       = auto_increment(); // permite que el token se incremente automaticamente

    return token;
}

void print_token(Token_t *token) {
    if (token == NULL) {
        printf_color("Token: NULL\n");
        return;
    }
    printf_color("Token[%s]: \"%s\", Type: %d\n", token->name_token, (char*)token->value, token->type);
}

#endif