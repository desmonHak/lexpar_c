#include "global.h"
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

Token_build_t* token_analysis (Lexer_t *lexer) {
    /*
        Los elementos de tipo `Token_build_t*` devueltos por esta funcion deben ser liberados
        con free por parte del programador.
     */
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , token_analysis)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        lexer);
    Token_build_t* self;


    repeat_switch:
        if (isdigit(lexer->chartter)) return lexer_parser_number(lexer);

        switch (lexer->chartter)
        {
        case '"':
        case '\'':
            return lexer_parser_string(lexer);
        case '{':
        case '}':
        case ':':
        case ',':
            self = init_token_build((Object)&(lexer->chartter));
            self->token = get_token(lexer, (const char[]){lexer->chartter, 0});
            lexer_advance(lexer);
            return self;
        case '\n':
        case ' ':
            lexer_advance(lexer);
            goto repeat_switch;
        default:
            printf_color("no definido %d %c\n", lexer->chartter, lexer->chartter);
            return NULL; 
        }

}

int main() {
    #ifdef _WIN32
        #include <windows.h>
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    const char datos[] =
    "{"
    "   \"name\": \"Juan\","
    "    \"age\": 20,\n"
    "}"
    ;
    Lexer_t lexer = init_lexer(datos, sizeof(datos));

    func_auto_increment func = inc_token;

    const position positions_tokens[] = {
        push_token(&lexer, create_token(create_name_token(token_llave_init),    "{",                                 token_llave_init)),
        push_token(&lexer, create_token(create_name_token(token_llave_end),     "}",                                  token_llave_end)),
        push_token(&lexer, create_token(create_name_token(token_coma),          ",",                                        inc_token)),
        push_token(&lexer, create_token(create_name_token(token_doble_punto),   ":",                                        inc_token)),
        push_token(&lexer, create_token(create_name_token(token_number),        build_token_special(TOKEN_NUMBER),          inc_token)),
        push_token(&lexer, create_token(create_name_token(token_id),            build_token_special(TOKEN_ID),              inc_token)),
        push_token(&lexer, create_token(create_name_token(token_eof),           build_token_special(TOKEN_EOF),             inc_token)),
        push_token(&lexer, create_token(create_name_token(token_string_simple), build_token_special(TOKEN_STRING_SIMPLE),   inc_token)),
        push_token(&lexer, create_token(create_name_token(token_string_double), build_token_special(TOKEN_STRING_DOUBLE),   inc_token))
    };

    //print_tokens(&lexer);

    // construir el lexer con los tokens
    build_lexer(&lexer);

    print_Token_build(&lexer, token_analysis);
    
    Ast_t *ast = init_ast(&lexer);

    const char* expressions[][4] = {
        {build_token_special(TOKEN_STRING_DOUBLE),  ":", build_token_special(TOKEN_STRING_DOUBLE), NULL},
        {build_token_special(TOKEN_NUMBER),         ":", build_token_special(TOKEN_STRING_DOUBLE), NULL},
        {build_token_special(TOKEN_STRING_DOUBLE),  ":", build_token_special(TOKEN_NUMBER), NULL},
        {build_token_special(TOKEN_NUMBER),         ":", build_token_special(TOKEN_NUMBER), NULL},

        {build_token_special(TOKEN_NUMBER),         ":", "{", NULL},
    };

    // Itera sobre el array y crea las expresiones
    for (size_t i = 0; i < sizeof(expressions) / sizeof(expressions[0]); i++) {
        add_expression_to_ast(ast, create_expression(ast, expressions[i][0], expressions[i][1], expressions[i][2], expressions[i][3]));
    }

    print_tokens(&lexer);
    print_ast(ast);


    int sequence[] = {4, 3, 8};
    size_t seq_size = sizeof(sequence) / sizeof(sequence[0]);

    if (is_sequence_in_ast(ast, sequence, seq_size)) {
        printf("La secuencia está presente en el AST.\n");
    } else {
        printf("La secuencia no está presente en el AST.\n");
    }

    // Liberar la memoria del AST y el Lexer
    freeAst(ast);
    free_lexer(&lexer);


    puts("exit");
    puts(datos);
    return 0;
}