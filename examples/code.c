#include "global.h"

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

int calc_ast_vals(ast_t *ast, Lexer_t *lexer) {
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
        free(tok); // liberar el Token_build_t
    }
    free(tok); // liberar el Token_build_t

    return val;
}
// Crea un nodo de operador con dos hijos
ast_node_t* create_op_node(const char* op, ast_node_t* left, ast_node_t* right) {
    ast_node_t* node = create_ast_node_t((void*)op);
    push_back_a(node->ramas, left);
    push_back_a(node->ramas, right);
    return node;
}

// Crea un nodo de número (hoja)
ast_node_t* create_num_node(const char* num_str) {
    return create_ast_node_t((void*)num_str);
}

// Construye el AST para la expresión (11 + 23) - (4 * 10) / 2
ast_node_t* build_example_ast() {
    ast_node_t* n11 = create_num_node("11");
    ast_node_t* n23 = create_num_node("23");
    ast_node_t* n4  = create_num_node("4");
    ast_node_t* n10 = create_num_node("10");
    ast_node_t* n2  = create_num_node("2");

    ast_node_t* plus = create_op_node("+", n11, n23);      // (11 + 23)
    ast_node_t* mul  = create_op_node("*", n4, n10);       // (4 * 10)
    ast_node_t* minus = create_op_node("-", plus, mul);    // (11 + 23) - (4 * 10)
    ast_node_t* root = create_op_node("/", minus, n2);     // ((11 + 23) - (4 * 10)) / 2

    return root;
}
int eval_ast(const ast_node_t* node) {
    if (!node) return 0;
    // Si no tiene hijos, es un número
    if (size_a(node->ramas) == 0) {
        return atoi((const char*)node->data);
    }
    // Si tiene hijos, es un operador
    const char* op = (const char*)node->data;
    int left = eval_ast(get_element_a(node->ramas, 0));
    int right = eval_ast(get_element_a(node->ramas, 1));
    if (strcmp(op, "+") == 0) return left + right;
    if (strcmp(op, "-") == 0) return left - right;
    if (strcmp(op, "*") == 0) return left * right;
    if (strcmp(op, "/") == 0) return left / right;
    printf("Operador desconocido: %s\n", op); return 0;
}
int main() {
    ast_node_t* ast = build_example_ast();

    printf("Árbol de la expresión:\n");
    print_ast_ascii(ast, "", 1);

    int resultado = eval_ast(ast);
    printf("Resultado: %d\n", resultado);

    free_ast_t(ast, NULL); // O la función de liberación que uses
    return 0;
}
