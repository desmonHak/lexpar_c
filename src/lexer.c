#ifndef LEXPAR_LEXER_C
#define LEXPAR_LEXER_C

#include "../include/lexer.h"

Lexer_t init_lexer() {
    return (Lexer_t){
        .list_id_tokens = createLinkedList(),
        .hash_table     = NULL
    };
}

/*
Permite añadir un token a la lista de tokens
*/
const position push_token(Lexer_t *lexer, Token_t *token) {
    insertNode(lexer->list_id_tokens, token);
}

void print_tokens(Lexer_t *lexer) {
    Node *current = lexer->list_id_tokens->head;
    while (current != NULL) {
        Token_t *token = (Token_t *)current->data;
        print_token(token);
        current = current->next;
    }
}

void build_lexer(Lexer_t *lexer) {
    const position size_list_tokens = size_v(lexer->list_id_tokens);
    if (size_list_tokens == 0){
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "#{FG:red}Error: #{FG:white}No se han encontrado tokens\n");
        return;
    }
    // hacer una tabla de hash para todos los tokens, con el tamaño de la lista de tokens
    lexer->hash_table = createHashTable(size_list_tokens);

    for (position i = 0; i < size_list_tokens; i++) {
        // curent = list_id_tokens[i]
        Token_t*    current = get_element_v(lexer->list_id_tokens, i);
        if (current == NULL) {
            DEBUG_PRINT(DEBUG_LEVEL_ERROR, "#{FG:red}Error: #{FG:white}No se ha podido obtener el token %d\n", i);
            continue;
        }
        // añadir a la tabla de hash cada uno de los tokens con sus valores
        put(lexer->hash_table, current->value, current);
    }
}

Token_t *get_token(Lexer_t *lexer, const char *value) {
    return (Token_t *)get(lexer->hash_table, value);
}

#endif