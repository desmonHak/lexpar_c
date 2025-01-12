#ifndef AST_C
#define AST_C


#include "../include/ast.h"

ASTNode* createNode(position value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for ASTNode");
        exit(EXIT_FAILURE);
    }
    node->value = value;
    node->ramas = createLinkedList();
    return node;
}

ASTNode* createAST(position rootValue, ...) {
    va_list args;
    va_start(args, rootValue);

    ASTNode* root = createNode(rootValue);
    ASTNode* current = root;

    position value;
    while ((value = va_arg(args, position)) != -1) {  
        ASTNode* newNode = createNode(value);
        push_back_v(current->ramas, newNode);
        current = newNode;
    }

    va_end(args);
    return root;
}

ASTNode* cloneAST(ASTNode* node) {
    if (node == NULL) return NULL;

    ASTNode* newNode = createNode(node->value);
    for (size_t i = 0; i < size_v(node->ramas); i++) {
        ASTNode* child = get_element_v(node->ramas, i);
        push_back_v(newNode->ramas, cloneAST(child));
    }

    return newNode;
}

void set_element_v(LinkedList* list, size_t index, ASTNode* newNode) {
    if (list == NULL || index >= size_v(list)) {
        fprintf(stderr, "Error: invalid index or list is NULL\n");
        return;
    }

    Node* current = list->head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }

    current->data = newNode;
}

void replace_element_v(LinkedList* list, size_t index, ASTNode* newNode) {
    if (list == NULL || index >= size_v(list)) {
        fprintf(stderr, "Error: invalid index or list is NULL\n");
        return;
    }

    ASTNode* oldNode = get_element_v(list, index);


    free(oldNode); 
    set_element_v(list, index, newNode);  
}

// Función para fusionar dos árboles de sintaxis abstracta (ASTs)
ASTNode* mergeASTs(ASTNode* ast1, ASTNode* ast2) {
    if (ast1 == NULL) return cloneAST(ast2);  // Si ast1 es NULL, clonar ast2
    if (ast2 == NULL) return cloneAST(ast1);  // Si ast2 es NULL, clonar ast1

    // Si los valores raíz son iguales, fusionar las ramas
    if (ast1->value == ast2->value) {
        ASTNode* mergedNode = createNode(ast1->value);  // Nodo con el valor común

        size_t size1 = size_v(ast1->ramas);
        size_t size2 = size_v(ast2->ramas);

        // Fusionar las ramas de ast1
        for (size_t i = 0; i < size1; i++) {
            ASTNode* branch1 = get_element_v(ast1->ramas, i);
            push_back_v(mergedNode->ramas, cloneAST(branch1));  // Clonamos las ramas de ast1
        }

        // Fusionar las ramas de ast2
        for (size_t j = 0; j < size2; j++) {
            ASTNode* branch2 = get_element_v(ast2->ramas, j);
            bool found = false;

            // Buscar si ya existe una rama con el mismo valor en mergedNode
            for (size_t k = 0; k < size_v(mergedNode->ramas); k++) {
                ASTNode* existingBranch = get_element_v(mergedNode->ramas, k);
                if (existingBranch->value == branch2->value) {
                    // Fusionar las ramas si tienen el mismo valor
                    ASTNode* mergedBranch = mergeASTs(existingBranch, branch2);
                    replace_element_v(mergedNode->ramas, k, mergedBranch);  // Reemplazar con la rama fusionada
                    found = true;
                    break;  // Ya hemos encontrado y fusionado la rama, no hace falta más búsqueda
                }
            }

            // Si no se encontró una rama correspondiente, agregarla como nueva
            if (!found) {
                push_back_v(mergedNode->ramas, cloneAST(branch2));
            }
        }

        return mergedNode;
    } else {
        // Si los valores raíz son diferentes, necesitamos crear un nuevo nodo y agregar ambos árboles como ramas separadas
        ASTNode* mergedNode = createNode(-1);  // -1 representa una divergencia
        push_back_v(mergedNode->ramas, cloneAST(ast1));  // Agregar ast1
        push_back_v(mergedNode->ramas, cloneAST(ast2));  // Agregar ast2
        return mergedNode;
    }
}


void print_repeated_values_in_ast(ASTNode* node, LinkedList* seen_values) {
    if (node == NULL) return;


    int is_duplicate = 0;
    Node* current = seen_values->head;
    while (current) {
        if (current->data == (void*)node->value) {
            is_duplicate = 1;
            break;
        }
        current = current->next;
    }

    if (is_duplicate) {
        printf("Repeated value: %d\n", node->value);
    } else {
        push_back_v(seen_values, (void*)node->value);
    }

    size_t numRamas = size_v(node->ramas);
    for (size_t i = 0; i < numRamas; i++) {
        ASTNode* child = get_element_v(node->ramas, i);
        print_repeated_values_in_ast(child, seen_values);
    }
}

void print_repeated_values(Ast_t* ast) {
    if (ast->root == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: AST is empty\n");
        return;
    }
    LinkedList* seen_values = createLinkedList();
    printf("Repeated values in the AST:\n");
    print_repeated_values_in_ast(get_element_v(ast->root, 0), seen_values);
}


void printAST(ASTNode* node, int depth, char* prefix, int isLast) {
    if (node == NULL) return;

    printf("%s%s%d\n", prefix, isLast ? "└── " : "├── ", node->value);

    char newPrefix[256];
    snprintf(newPrefix, sizeof(newPrefix), "%s%s", prefix, isLast ? "    " : "│   ");

    size_t numRamas = size_v(node->ramas);
    for (size_t i = 0; i < numRamas; i++) {
        ASTNode* child = get_element_v(node->ramas, i);
        printAST(child, depth + 1, newPrefix, i == numRamas - 1);
    }
}

void freeASTNode(ASTNode* root) {
    if (!root) return;
    
    size_t numRamas = size_v(root->ramas);
    for (size_t i = 0; i < numRamas; i++) {
        freeASTNode(get_element_v(root->ramas, i));
    }
    
    freeLinkedList(root->ramas);
    free(root);
}
void freeAst(Ast_t* ast) {
    if (!ast) return;

    if (ast->root) {
        size_t numTrees = size_v(ast->root);
        for (size_t i = 0; i < numTrees; i++) {
            ASTNode* tree = get_element_v(ast->root, i);
            freeASTNode(tree);  // Liberar cada árbol en la lista raíz
        }
        freeLinkedList(ast->root);  // Liberar la lista raíz
    }

    free(ast);  // Liberar la estructura Ast_t
}


Ast_t* init_ast(Lexer_t* lexer) {
    Ast_t* ast = (Ast_t*)malloc(sizeof(Ast_t));
    if (!ast) {
        perror("Failed to allocate memory for Ast_t");
        exit(EXIT_FAILURE);
    }

    ast->root = createLinkedList();  // Iniciar root como lista vacía
    ast->lexer = lexer;

    return ast;
}
ASTNode* create_expression(Ast_t* ast, const char* token_init, ...) {
    Token_id root_id = ((Token_t*)get(ast->lexer->hash_table, token_init))->type;
    ASTNode* root = createNode(root_id);  // Crear el nodo raíz
    ASTNode* current = root;  // Inicializamos el nodo actual

    va_list args;
    va_start(args, token_init);

    const char* actual_val;
    while ((actual_val = va_arg(args, const char*)) != NULL) {
        Token_id token_id = ((Token_t*)get(ast->lexer->hash_table, actual_val))->type;
        ASTNode* new_node = createNode(token_id);

        // Verificar si el valor del nodo actual es igual al del nuevo nodo
        if (current->value == new_node->value) {
            // Si el valor es el mismo, fusionar las ramas
            push_back_v(current->ramas, new_node);
        } else {
            // Si el valor es diferente, avanzar al siguiente nivel
            push_back_v(current->ramas, new_node);
            current = new_node;  // Actualizar el nodo actual
        }
    }

    va_end(args);
    return root;
}



void add_expression_to_ast(Ast_t* ast, ASTNode* expression) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(void, add_expression_to_ast)
            TYPE_DATA_DBG(Ast_t*, "ast = %p")
            TYPE_DATA_DBG(ASTNode*, "expression = %p")
        END_TYPE_FUNC_DBG,
        ast, expression);

    if (ast->root == NULL) {
        // Si el root está vacío, inicializarlo como una lista y agregar la primera expresión
        ast->root = createLinkedList();
        push_back_v(ast->root, expression);
        return;
    }

    // Verificar si existe un árbol en root con el mismo valor raíz
    size_t numTrees = size_v(ast->root);
    for (size_t i = 0; i < numTrees; i++) {
        ASTNode* existingTree = get_element_v(ast->root, i);

        if (existingTree->value == expression->value) {
            // Si los valores raíz coinciden, fusionar los árboles
            ASTNode* mergedTree = mergeASTs(existingTree, expression);
            replace_element_v(ast->root, i, mergedTree);
            return;
        }
    }

    // Si no hay coincidencia en las raíces, agregar como un nuevo árbol independiente
    push_back_v(ast->root, expression);
}
int check_sequence_in_ast_array(ASTNode* node, int* sequence, size_t seq_size, size_t index) {
    if (node == NULL || index >= seq_size) return 0;

    // Compara el valor del nodo con el valor de la secuencia en la posición actual
    if (node->value != sequence[index]) {
        return 0;  // Si no coinciden, no se encuentra la secuencia
    }

    // Si hemos recorrido toda la secuencia, significa que la encontramos
    if (index == seq_size - 1) {
        return 1;
    }

    // Si aún no hemos recorrido toda la secuencia, debemos seguir buscando en las ramas
    size_t numRamas = size_v(node->ramas);
    for (size_t i = 0; i < numRamas; i++) {
        ASTNode* child = get_element_v(node->ramas, i);
        if (check_sequence_in_ast(child, sequence, seq_size, index + 1)) {
            return 1;  // Secuencia encontrada en alguna rama
        }
    }

    return 0;  // No se encontró la secuencia en las ramas
}

// Función pública que puedes llamar para verificar la secuencia
int is_sequence_in_ast_array(Ast_t* ast, int* sequence, size_t seq_size) {
    if (ast->root == NULL) {
        return 0;  // El AST está vacío
    }

    // Iteramos sobre cada árbol en la raíz del AST
    size_t numTrees = size_v(ast->root);
    for (size_t i = 0; i < numTrees; i++) {
        ASTNode* existingTree = get_element_v(ast->root, i);
        if (check_sequence_in_ast_array(existingTree, sequence, seq_size, 0)) {
            return 1;  // Secuencia encontrada
        }
    }

    return 0;  // No se encontró la secuencia en ningún árbol
}

int check_sequence_in_ast_vector(ASTNode* node, LinkedList* sequence, size_t seq_size, size_t index) {
    if (node == NULL || index >= seq_size) return 0;

    // Compara el valor del nodo con el valor de la secuencia en la posición actual
    if (node->value != *((const position*)get_element_v(sequence, index))) {
        return 0;  // Si no coinciden, no se encuentra la secuencia
    }

    // Si hemos recorrido toda la secuencia, significa que la encontramos
    if (index == seq_size - 1) {
        return 1;
    }

    // Si aún no hemos recorrido toda la secuencia, debemos seguir buscando en las ramas
    size_t numRamas = size_v(node->ramas);
    for (size_t i = 0; i < numRamas; i++) {
        ASTNode* child = get_element_v(node->ramas, i);
        if (check_sequence_in_ast_vector(child, sequence, seq_size, index + 1)) {
            return 1;  // Secuencia encontrada en alguna rama
        }
    }

    return 0;  // No se encontró la secuencia en las ramas
}

// Función pública que puedes llamar para verificar la secuencia
int is_sequence_in_ast_vector(Ast_t* ast, LinkedList* sequence, size_t seq_size) {
    if (ast->root == NULL || sequence == NULL) {
        return 0;  // El AST está vacío o la secuencia está vacía
    }

    // Iteramos sobre cada árbol en la raíz del AST
    size_t numTrees = size_v(ast->root);
    for (size_t i = 0; i < numTrees; i++) {
        ASTNode* existingTree = get_element_v(ast->root, i);
        if (check_sequence_in_ast_vector(existingTree, sequence, seq_size, 0)) {
            return 1;  // Secuencia encontrada
        }
    }

    return 0;  // No se encontró la secuencia en ningún árbol
}



void print_ast(Ast_t* ast) {
    if (ast->root == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: AST is empty\n");
        return;
    }
    printf("AST Structure:\n");
    Node* current = ast->root->head;
    while (current) {
        printAST(current->data, 0, "", 1);
        current = current->next;
    }
}


#endif