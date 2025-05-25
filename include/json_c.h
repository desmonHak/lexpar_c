#ifndef JSON_C_H
#define JSON_C_H

#include "global.h"
#include "ast.h"
#include "token.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum names_tokens {
    token_llave_init,
    token_llave_end,
    token_coma,
    token_doble_punto,
    token_corchete_init,
    token_corchete_end,
    token_true,
    token_false,
    token_null
} names_tokens;

// --- API PRINCIPAL DE LA LIBRERÍA JSON ---
#ifdef __cplusplus
extern "C" {
#endif

// Analiza el siguiente token JSON del lexer
Token_build_t* json_token_analysis(Lexer_t *lexer);

// Parsea un valor JSON (objeto, array, string, número, true, false, null)
ast_node_t* parse_json_value(Lexer_t* lexer);

// Parsea un objeto JSON
ast_node_t* parse_json_object(Lexer_t* lexer);

// Parsea un array JSON
ast_node_t* parse_json_array(Lexer_t* lexer);

// Obtiene el valor de una clave en un objeto JSON AST
ast_node_t* get_json_value_by_key(ast_node_t* obj, const char* key);

// Obtiene el nodo del array en la posición idx
ast_node_t* get_json_array_element(ast_node_t* array, size_t idx);

// Busca un valor por ruta tipo "address/city"
ast_node_t* get_json_value_by_path(ast_node_t* node, const char* path);

// Busca todos los nodos con un valor dado
void find_nodes_by_value(ast_node_t* node, const char* value);

// Imprime un nodo string (para AST)
void print_node_str(ast_t* node, int depth, char* prefix, int is_last);

// Obtiene el string de un nodo
char* get_str_from_node(ast_t* node);

// Función auxiliar para obtener el string del siguiente token (peek)
const char* peek_token_str(Lexer_t* lexer);

void json_init_lexer(Lexer_t* lexer);

// Carga y parsea un archivo JSON, devuelve el AST o NULL si falla
ast_node_t* json_load_file(const char* filename);

// Libera el AST y recursos asociados
void json_free(const ast_node_t* root);

// Devuelve el nodo AST correspondiente a una ruta tipo "grades/0/title"
ast_node_t* json_get_node(ast_node_t* root, const char* path);

// Devuelve el string de un nodo por ruta (o NULL si no existe o no es string)
const char* json_get_string(ast_node_t* root, const char* path);

// Devuelve el número (double) de un nodo por ruta (o NAN si no existe o no es numérico)
double json_get_number(ast_node_t* root, const char* path);

// Devuelve el booleano de un nodo por ruta (1=true, 0=false, -1 si no es booleano)
int json_get_bool(ast_node_t* root, const char* path);


#ifdef __cplusplus
}
#endif

#endif // JSON_C_H
