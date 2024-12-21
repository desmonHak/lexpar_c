#include "include/parser.h"

typedef enum names_tokens {
    token_div,
    token_mul,
    token_sub,
    token_add
} names_tokens;

const Token_id inc_token(void) {
    static Token_id my_token_id = token_add+1;
    return my_token_id++;
}

Token_build_t* token_analysis (Lexer_t *lexer) {
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
        case '+':
        case '-':
        case '/':
        case '*':
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

int calc_ast_vals(Ast_t *ast, Lexer_t *lexer) {
    // restaurar el lexer, es necesario para poder seguir operando con el si alguien o algo lo modifico
    restore_lexer(lexer);

    int val = 0;

    // obtener el ID de los tokens de tipo numero:
    Token_id token_number = ((Token_t *)get(lexer->hash_table, build_token_special(TOKEN_NUMBER)))->type;
    Token_id token_eof    = ((Token_t*)get(lexer->hash_table, build_token_special(TOKEN_EOF)))->type;

    int operador = 0;

    // mientras no se llegue al final de los tokens
    Token_build_t* tok;
    for (int i = 0;
        (
            tok = lexer_next_token(lexer, token_analysis)
        )->token->type != token_eof;
        i++
        ) {
            
        print_token(tok->token);
        printf_color("token: %s\n", tok->token->name_token);
        if (tok->token->type == token_number) {
            int number = atoll((const char*)tok->value_process);
            if (i == 0) {val = number; continue;} // si es la primera vez, asignar el valor
            printf_color("numero: %d\n", number);
            switch (operador)
            {
                case token_add:
                    printf_color( "operado: %d + %d\n", val, number);
                    val += number;
                    break;
                case token_sub:
                    printf_color( "operado: %d - %d\n", val, number);
                    val -= number;
                    break;
                case token_div:
                    printf_color( "operado: %d / %d\n", val, number);
                    val /= number;
                    break;
                case token_mul:
                    printf_color( "operado: %d * %d\n", val, number);
                    val *= number;
                    break;
                default:
                    puts("sintax error");
                    break;
            }
        } else {
            operador = tok->token->type;
        }
    }
    

    return val;
}


int main() {
    #ifdef _WIN32
        #include <windows.h>
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    const char datos[] = "11+23-4*10";
    Lexer_t lexer = init_lexer(datos, sizeof(datos));

    func_auto_increment func = inc_token;

    const position positions_tokens[] = {
        push_token(&lexer, create_token(create_name_token(token_div),   "-",                                                token_sub)),
        push_token(&lexer, create_token(create_name_token(token_add),   "+",                                                token_add)),
        push_token(&lexer, create_token(create_name_token(token_mul),   "*",                                                token_mul)),
        push_token(&lexer, create_token(create_name_token(token_sub),   "/",                                                token_div)),
        push_token(&lexer, create_token(create_name_token(token_number),        build_token_special(TOKEN_NUMBER),          inc_token)),
        push_token(&lexer, create_token(create_name_token(token_id),            build_token_special(TOKEN_ID),              inc_token)),
        push_token(&lexer, create_token(create_name_token(token_eof),           build_token_special(TOKEN_EOF),             inc_token)),
        push_token(&lexer, create_token(create_name_token(token_string_simple), build_token_special(TOKEN_STRING_SIMPLE),   inc_token)),
        push_token(&lexer, create_token(create_name_token(token_string_double), build_token_special(TOKEN_STRING_DOUBLE),   inc_token))
    };

    // añadir tokens al lexer
    

    //print_tokens(&lexer);

    // construir el lexer con los tokens
    build_lexer(&lexer);

    print_Token_build(&lexer, token_analysis);
    
    Ast_t *ast = init_ast(&lexer);

    const char* expressions[][3] = {
        {build_token_special(TOKEN_NUMBER), "-", NULL},
        {build_token_special(TOKEN_NUMBER), "/", NULL},
        {build_token_special(TOKEN_NUMBER), "+", NULL},
        {build_token_special(TOKEN_NUMBER), "*", NULL},
        {build_token_special(TOKEN_NUMBER), build_token_special(TOKEN_EOF), NULL},
    };

    // Itera sobre el array y crea las expresiones
    for (size_t i = 0; i < sizeof(expressions) / sizeof(expressions[0]); i++) {
        add_expression_to_ast(ast, create_expression(ast, expressions[i][0], expressions[i][1], expressions[i][2]));
    }

    print_tokens(&lexer);
    print_ast(ast);

    
    printf("valor de la operacion: %d\n", calc_ast_vals(ast, &lexer));

    puts("exit");
    puts(datos);
    return 0;
}