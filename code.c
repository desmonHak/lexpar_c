#include "include/lexer.h"

typedef enum names_tokens {
    token_llave_init,
    token_llave_end,
    token_coma,
    token_doble_punto
} names_tokens;

const Token_id inc_token(void) {
    static Token_id my_token_id = token_llave_end+1;
    return my_token_id++;
}

const Token_build_t* token_analysis (Lexer_t *lexer) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , token_analysis)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        lexer);
    Token_build_t* self;
    switch (lexer->chartter)
    {
    case '{':
    case '}':
    case ':':
    case ',':
        self = init_token_build(&(lexer->chartter));
        self->token = get_token(lexer, lexer->chartter);
        lexer_advance(lexer);
        return self;
    default:
        puts("no definido");
        return NULL; 
    }

}

int main() {
    const char datos[] =
    "{"
    "   \"name\": \"Juan\","
    "    \"age\": 20,\n"
    "}"
    ;
    Lexer_t lexer = init_lexer(datos, sizeof(datos));

    func_auto_increment func = inc_token;

    // añadir tokens al lexer
    push_token(&lexer, create_token(create_name_token(token_llave_init), "{", token_llave_init));
    push_token(&lexer, create_token(create_name_token(token_llave_end),  "}", token_llave_end));
    push_token(&lexer, create_token(create_name_token(token_coma), ",", inc_token));
    push_token(&lexer, create_token(create_name_token(token_doble_punto), ":", inc_token));
    push_token(&lexer, create_token(create_name_token(token_number), build_token_special(TOKEN_NUMBER), inc_token));
    push_token(&lexer, create_token(create_name_token(token_id), build_token_special(TOKEN_ID), inc_token));
    push_token(&lexer, create_token(create_name_token(token_eof), build_token_special(TOKEN_EOF), inc_token));

    print_tokens(&lexer);

    // construir el lexer con los tokens
    build_lexer(&lexer);

    print_Token_build(&lexer, token_analysis);

    puts("exit");
    return 0;
}