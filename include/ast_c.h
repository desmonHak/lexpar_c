#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#ifndef _ExceptionHandler_WIN_
    #define _ExceptionHandler_WIN_
#endif

#include "global.h"

#include "ast.h"
#include "time_code.h"
#include "hash-table.h"
#include "vector-list.h"


#ifndef Lexer_t
typedef struct Lexer_t {
    LinkedList  *list_id_tokens; // contiene todos los tockens deseados
    HashTable       *hash_table; // contiene los tokens y su valor
    unsigned char      chartter; // contiene el caracter actual
    uint64_t              index; // contiene el indice del caracter actual
    uint64_t               size; // tamaño de la cadena
    const char*            data; // datos a analizar
} Lexer_t;
#define Lexer_t Lexer_t
#endif

typedef struct seq_search_ctx{
    const int* sequence;
    size_t seq_size;
    size_t current_pos;
    int (*get_value)(void*);
} seq_search_ctx;


// ================== Detección de Duplicados ================== //
typedef struct dup_check_ctx{
    ArrayList* seen;
    int (*get_key)(void*);
} dup_check_ctx;

ast_node_t* createAST(position rootValue, ...) ;

#endif