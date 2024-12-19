#ifndef LEXPAR_LEXER_H
#define LEXPAR_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef _ExceptionHandler_WIN_
    #define _ExceptionHandler_WIN_
#endif

#include "../lib/structs_data_C/DebugLibC/time_code.h"
#include "../lib/structs_data_C/hash-table.h"
#include "../lib/structs_data_C/vector-list.h"
#include "token.h"

typedef struct Lexer_t {
    LinkedList *list_id_tokens; // contiene todos los tockens deseados
    HashTable  *hash_table;     // contiene los tokens y su valor
} Lexer_t;

#include "../src/lexer.c"
#endif