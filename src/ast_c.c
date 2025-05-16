#ifndef AST_C
#define AST_C

#include "ast_c.h"
/*
* @brief Crea un AST lineal a partir de una secuencia de valores tipo position.
*
* @param rootValue Valor del nodo raíz.
* @param ... Secuencia de valores (tipo position), terminando con (position)-1.
* @return ast_node_t* Nodo raíz del AST creado.
*/
ast_node_t* createAST(position rootValue, ...) {
    /**
    // Suponiendo que position es int
    ast_node_t* ast = createAST(10, 20, 30, 40, -1);
    // Crea un AST: 10 -> 20 -> 30 -> 40
    */
    va_list args;
    va_start(args, rootValue);

    ast_node_t* root = create_ast_node_t((void*)(intptr_t)rootValue);
    ast_node_t* current = root;

    position value;
    while ((value = va_arg(args, position)) != (position)-1) {
        ast_node_t* newNode = create_ast_node_t((void*)(intptr_t)value);
        push_back_a(current->ramas, newNode);
        current = newNode;
    }

    va_end(args);
    return root;
}

// Reemplaza el elemento en la posición i de un ArrayList por vall
#define replace_element_a(arr, i, vall) ((arr)->Array[i] = (vall))

// ================== Función de Merge ================== //
ast_t* merge_ast(ast_t* ast1, ast_t* ast2,
                int (*compare)(const void*, const void*),
                void* (*clone_data)(void*)) {
    if (!ast1) return clone_ast(ast2);
    if (!ast2) return clone_ast(ast1);

    if (compare(ast1->data, ast2->data) == 0) {
        ast_t* merged = create_ast_node_t(clone_data(ast1->data));

        // Fusionar ramas de ast1
        for (size_t i = 0; i < size_a(ast1->ramas); ++i) {
            ast_t* branch = (ast_t*)get_element_a(ast1->ramas, i);
            push_back_a(merged->ramas, clone_ast(branch));
        }

        // Fusionar ramas de ast2
        for (size_t j = 0; j < size_a(ast2->ramas); ++j) {
            ast_t* branch = (ast_t*)get_element_a(ast2->ramas, j);
            bool exists = false;

            for (size_t k = 0; k < size_a(merged->ramas); ++k) {
                ast_t* existing = (ast_t*)get_element_a(merged->ramas, k);
                if (compare(existing->data, branch->data) == 0) {
                    ast_t* merged_branch = merge_ast(existing, branch, compare, clone_data);
                    replace_element_a(merged->ramas, k, merged_branch);
                    exists = true;
                    break;
                }
            }

            if (!exists) push_back_a(merged->ramas, clone_ast(branch));
        }
        return merged;
    } else {
        ast_t* container = create_ast_node_t(NULL);
        push_back_a(container->ramas, clone_ast(ast1));
        push_back_a(container->ramas, clone_ast(ast2));
        return container;
    }
}



static void check_duplicates_cb(const ast_t* node, dup_check_ctx* ctx) {
    int key = ctx->get_key(node->data);

    for (size_t i = 0; i < size_a(ctx->seen); ++i) {
        if (*(int*)get_element_a(ctx->seen, i) == key) {
            printf("Valor duplicado: %d\n", key);
            return;
        }
    }

    int* new_key = malloc(sizeof(int));
    *new_key = key;
    push_back_a(ctx->seen, new_key);
}


// Función recursiva específica para duplicados
void find_duplicates_in_ast(const ast_t* node, dup_check_ctx* ctx) {
    if (!node) return;

    int key = ctx->get_key(node->data);
    for (size_t i = 0; i < size_a(ctx->seen); ++i) {
        if (*(int*)get_element_a(ctx->seen, i) == key) {
            printf("Valor duplicado: %d\n", key);
            break;
        }
    }
    int* new_key = malloc(sizeof(int));
    *new_key = key;
    push_back_a(ctx->seen, new_key);

    for (size_t i = 0; i < size_a(node->ramas); ++i) {
        find_duplicates_in_ast(get_element_a(node->ramas, i), ctx);
    }
}

void find_duplicates(ast_t* ast, int (*get_key)(void*)) {
    if (!ast) return;
    dup_check_ctx ctx = {
        .seen = createArrayList(10, free),
        .get_key = get_key
    };
    find_duplicates_in_ast(ast, &ctx);
    freeArrayList(&ctx.seen);
}

// ================== Construcción de Expresiones ================== //
ast_t* build_expression(const char* root_token, ...) {
    va_list args;
    va_start(args, root_token);

    ast_t* root = create_ast_node_t((void*)root_token);
    ast_t* current = root;

    const char* token;
    while ((token = va_arg(args, const char*)) != NULL) {
        ast_t* new_node = create_ast_node_t((void*)token);

        if (strcmp((char*)current->data, (char*)new_node->data) == 0) {
            push_back_a(current->ramas, new_node);
        } else {
            push_back_a(current->ramas, new_node);
            current = new_node;
        }
    }

    va_end(args);
    return root;
}

// ================== Integración en AST Principal ================== //
void integrate_expression(ast_t* main_ast, ast_t* expr,
                         int (*compare)(const void*, const void*)) {
    if (!main_ast->ramas) {
        main_ast->ramas = createArrayList(5, NULL);
        push_back_a(main_ast->ramas, expr);
        return;
    }

    for (size_t i = 0; i < size_a(main_ast->ramas); ++i) {
        ast_t* existing = (ast_t*)get_element_a(main_ast->ramas, i);

        if (compare(existing->data, expr->data) == 0) {
            ast_t* merged = merge_ast(existing, expr, compare, (void * (*)(void *))strdup);
            replace_element_a(main_ast->ramas, i, merged);
            return;
        }
    }

    push_back_a(main_ast->ramas, expr);
}


static int check_sequence_cb(ast_t* node, seq_search_ctx* ctx) {
    int node_value = ctx->get_value(node->data);

    if (node_value == ctx->sequence[ctx->current_pos]) {
        if (ctx->current_pos == ctx->seq_size - 1) return 1;

        ctx->current_pos++;
        for (size_t i = 0; i < size_a(node->ramas); ++i) {
            if (check_sequence_cb((ast_t*)get_element_a(node->ramas, i), ctx)) {
                return 1;
            }
        }
        ctx->current_pos--;
    }
    return 0;
}

int contains_sequence(ast_t* ast, const int* sequence, size_t seq_size,
                     int (*get_value)(void*)) {
    seq_search_ctx ctx = {
        .sequence = sequence,
        .seq_size = seq_size,
        .get_value = get_value
    };

    for (size_t i = 0; i < size_a(ast->ramas); ++i) {
        ctx.current_pos = 0;
        if (check_sequence_cb((ast_t*)get_element_a(ast->ramas, i), &ctx)) {
            return 1;
        }
    }
    return 0;
}

#endif
