#ifndef LEXPAR_LEXER_H
#define LEXPAR_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>

#ifndef _ExceptionHandler_WIN_
    #define _ExceptionHandler_WIN_
#endif

#include "time_code.h"
#include "hash-table.h"
#include "vector-list.h"

#include "global.h"

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

// tokens predefinidos
static Token_t * token_number = NULL;
static Token_t * token_eof    = NULL;
static Token_t * token_id     = NULL;

typedef Token_build_t* (*func_token_analysis)(Lexer_t* lexer);

Token_build_t* lexer_advance_with(Lexer_t* lexer, Token_build_t* token);
Token_build_t* lexer_parser_number(Lexer_t* lexer);
Token_build_t* lexer_parser_id(Lexer_t* lexer);
Token_build_t* lexer_next_token(Lexer_t* lexer, func_token_analysis token_analysis);
Token_build_t* lexer_parser_string(Lexer_t* lexer);

Token_t *get_token(Lexer_t *lexer, const char *value);
Lexer_t init_lexer(const char* data, uint64_t size);

long long strtoll_plus(const char *str, char *endptr, int base);
const position push_token(Lexer_t *lexer, Token_t *token);
void print_Token_build(Lexer_t* lexer, func_token_analysis token_analysis);
void lexer_advance(Lexer_t* lexer);
void lexer_skip_whitespace(Lexer_t* lexer);
void print_tokens(Lexer_t *lexer);
void build_lexer(Lexer_t *lexer);
void free_lexer(Lexer_t *lexer);
void restore_lexer(Lexer_t* lexer);

#endif