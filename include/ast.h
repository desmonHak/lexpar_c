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
    position value;     // valor del nodo
    LinkedList * ramas; // ASTNode*
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

#define GET_MACRO(_1, _2, NAME, ...) NAME
#ifndef freeHashTable
#define freeHashTable(...) GET_MACRO(__VA_ARGS__, freeHashTable_all, freeHashTable_struct)(__VA_ARGS__)
#endif

#ifndef check_sequence_in_ast
#define check_sequence_in_ast(node, sequence, seq_size, index) _Generic((sequence),     \
    int*:               check_sequence_in_ast_array,                                    \
    ArrayList*:         check_sequence_in_ast_vector,                                   \
    default: check_sequence_in_ast_array)(node, sequence, seq_size, index)
#endif

#ifndef is_sequence_in_ast
#define is_sequence_in_ast(node, sequence, seq_size) _Generic((sequence),     \
    int*:               is_sequence_in_ast_array,                             \
    LinkedList*:         is_sequence_in_ast_vector,                            \
    default: is_sequence_in_ast_array)(node, sequence, seq_size)
#endif

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

int check_sequence_in_ast_array(ASTNode* node, int* sequence, size_t seq_size, size_t index);
int is_sequence_in_ast_array(Ast_t* ast, int* sequence, size_t seq_size);

int check_sequence_in_ast_vector(ASTNode* node, LinkedList* sequence, size_t seq_size, size_t index);
int is_sequence_in_ast_vector(Ast_t* ast, LinkedList* sequence, size_t seq_size);

#include "../src/ast.c"
#endif