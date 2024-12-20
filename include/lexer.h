#ifndef LEXPAR_LEXER_H
#define LEXPAR_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#ifndef _ExceptionHandler_WIN_
    #define _ExceptionHandler_WIN_
#endif

#include "../lib/structs_data_C/DebugLibC/time_code.h"
#include "../lib/structs_data_C/hash-table.h"
#include "../lib/structs_data_C/vector-list.h"
#include "token.h"

typedef struct Lexer_t {
    LinkedList  *list_id_tokens; // contiene todos los tockens deseados
    HashTable       *hash_table; // contiene los tokens y su valor
    unsigned char      chartter; // contiene el caracter actual
    uint64_t              index; // contiene el indice del caracter actual
    uint64_t               size; // tamaño de la cadena
    const char*            data; // datos a analizar
} Lexer_t;

// tokens predefinidos
static Token_t * token_number = NULL;
static Token_t * token_eof    = NULL;
static Token_t * token_id     = NULL;

typedef Token_build_t* (*func_token_analysis)(Lexer_t* lexer);

#include "../src/lexer.c"
#endif