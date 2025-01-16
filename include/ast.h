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


// Definición del nodo AST
typedef struct ASTNode {
    position value;
    LinkedList * ramas;
    //struct ASTNode *left;
    //struct ASTNode *right;
} ASTNode;

typedef struct Ast_t {
    LinkedList * root;
    Lexer_t* lexer;
} Ast_t;

typedef struct AstExpression {
    LinkedList  *list_id_tokens;
    size_t                 size;
} AstExpression;

ASTNode* createNode(position value);
ASTNode* cloneAST(ASTNode* node);
ASTNode* createAST(position rootValue, ...);
ASTNode* mergeASTs(ASTNode* ast1, ASTNode* ast2);
void printAST(ASTNode* node, int depth, char* prefix, int isLast);
void freeASTNode(ASTNode* root);
void freeAst(Ast_t* ast);
Ast_t* init_ast(Lexer_t* lexer);
ASTNode* create_expression(Ast_t* ast, const char* token_init, ...);
void add_expression_to_ast(Ast_t* ast, ASTNode* expression);
void print_ast(Ast_t* ast);
int is_sequence_in_ast(Ast_t* ast, int* sequence, size_t seq_size) ;

#endif