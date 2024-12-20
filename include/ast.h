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

#include "lexer.h"

#include "../lib/structs_data_C/DebugLibC/time_code.h"
#include "../lib/structs_data_C/hash-table.h"
#include "../lib/structs_data_C/vector-list.h"


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
void freeAST(ASTNode* root);
Ast_t* init_ast(Lexer_t* lexer);
ASTNode* create_expression(Ast_t* ast, const char* token_init, ...);
void add_expression_to_ast(Ast_t* ast, ASTNode* expression);
void print_ast(Ast_t* ast);
int is_sequence_in_ast(Ast_t* ast, int* sequence, size_t seq_size) ;

#include "../src/ast.c"
#endif