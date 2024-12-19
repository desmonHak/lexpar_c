#include "include/lexer.h"

typedef enum names_tokens {
    token_comment,
    token_number
} names_tokens;

const Token_id inc_token(void) {
    static Token_id my_token_id = token_number+1;
    return my_token_id++;
}

int main() {
    Lexer_t lexer = init_lexer();

    func_auto_increment func = inc_token;

    // añadir tokens al lexer
    push_token(&lexer, create_token(none_name, ">", token_number));
    push_token(&lexer, create_token(none_name, ";", func));
    push_token(&lexer, create_token(none_name, ".", func));
    push_token(&lexer, create_token(create_name_token(token_comment), "@comment", token_comment));

    // construir el lexer con los tokens
    build_lexer(&lexer);

    print_token(get_token(&lexer, ">"));

    print_tokens(&lexer);

    puts("exit");
    return 0;
}