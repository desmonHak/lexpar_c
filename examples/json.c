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

const Token_id inc_token(void) {
    static Token_id my_token_id = token_null + 1;
    return my_token_id++;
}

// --- Token analysis para JSON (soporta true, false, null) ---
Token_build_t* token_analysis(Lexer_t *lexer) {
    Token_build_t* self;
repeat_switch:
    if (isdigit(lexer->chartter) || lexer->chartter == '-' || lexer->chartter == '+') {
        // Reconoce números JSON (enteros y flotantes)
        size_t start = lexer->index;
        int has_dot = 0, has_exp = 0;
        if (lexer->chartter == '-' || lexer->chartter == '+') lexer_advance(lexer);
        while (isdigit(lexer->chartter)) lexer_advance(lexer);
        if (lexer->chartter == '.') {
            has_dot = 1;
            lexer_advance(lexer);
            while (isdigit(lexer->chartter)) lexer_advance(lexer);
        }
        if (lexer->chartter == 'e' || lexer->chartter == 'E') {
            has_exp = 1;
            lexer_advance(lexer);
            if (lexer->chartter == '-' || lexer->chartter == '+') lexer_advance(lexer);
            while (isdigit(lexer->chartter)) lexer_advance(lexer);
        }
        size_t len = lexer->index - start;
        char* num_str = (char*)malloc(len + 1);
        strncpy(num_str, lexer->data + start, len);
        num_str[len] = 0;
        Token_build_t* tok = init_token_build(num_str);
        tok->token = get_token(lexer, build_token_special(TOKEN_NUMBER));
        return tok;
    }

    // Reconoce true, false, null
    if (strncmp(&lexer->data[lexer->index], "true", 4) == 0) {
        lexer->index += 4;
        lexer->chartter = lexer->data[lexer->index];
        self = init_token_build(strdup("true"));
        self->token = get_token(lexer, "true");
        return self;
    }
    if (strncmp(&lexer->data[lexer->index], "false", 5) == 0) {
        lexer->index += 5;
        lexer->chartter = lexer->data[lexer->index];
        self = init_token_build(strdup("false"));
        self->token = get_token(lexer, "false");
        return self;
    }
    if (strncmp(&lexer->data[lexer->index], "null", 4) == 0) {
        lexer->index += 4;
        lexer->chartter = lexer->data[lexer->index];
        self = init_token_build(strdup("null"));
        self->token = get_token(lexer, "null");
        return self;
    }

    switch (lexer->chartter) {
        case '"':
        case '\'':
            return lexer_parser_string(lexer);
        case '{': case '}':
        case ':': case ',':
        case '[': case ']':
            self = init_token_build((Object)&(lexer->chartter));
            self->token = get_token(lexer, (const char[]){lexer->chartter, 0});
            lexer_advance(lexer);
            return self;
        case '\n': case ' ':
        case '\t': case '\r':
            lexer_advance(lexer);
            goto repeat_switch;
        default:
            printf("No definido %d %c\n", lexer->chartter, lexer->chartter);
            return NULL;
    }
}

// --- Impresión personalizada para nodos AST de strings ---
void print_node_str(ast_t* node, int depth, char* prefix, int is_last) {
    printf("%s%s%s\n", prefix, is_last ? "└── " : "├── ", (char*)node->data);
}

// --- Función para obtener el string de un nodo (para búsqueda por ruta) ---
char* get_str_from_node(ast_t* node) {
    return (char*)node->data;
}

// --- Búsqueda por valor (recorre todo el árbol y muestra nodos que coinciden) ---
void find_nodes_by_value(ast_t* node, const char* value) {
    if (!node) return;
    if (node->data && strcmp((char*)node->data, value) == 0) {
        printf("Nodo encontrado con valor \"%s\": %p\n", value, (void*)node);
    }
    size_t num_ramas = size_a(node->ramas);
    for (size_t i = 0; i < num_ramas; ++i) {
        find_nodes_by_value((ast_t*)get_element_a(node->ramas, i), value);
    }
}

// --- Prototipos ---
ast_node_t* parse_json_value(Lexer_t* lexer);
ast_node_t* parse_json_object(Lexer_t* lexer);
ast_node_t* parse_json_array(Lexer_t* lexer);

// --- peek_token_str robusto ---
const char* peek_token_str(Lexer_t* lexer) {
    // Backup solo los campos relevantes
    uint64_t old_index = lexer->index;
    unsigned char old_chartter = lexer->chartter;

    Token_build_t* tok = lexer_next_token(lexer, token_analysis);
    const char* str = strdup((const char*)tok->token->name_token);
    free(tok);

    lexer->index = old_index;
    lexer->chartter = old_chartter;
    return str;
}

// --- Parseo de valores JSON (objeto, array, string, número, true, false, null) ---
ast_node_t* parse_json_value(Lexer_t* lexer) {
    const char* peek = peek_token_str(lexer);

    if (strcmp(peek, "{") == 0) {
        free((void*)peek);
        return parse_json_object(lexer);
    } else if (strcmp(peek, "[") == 0) {
        free((void*)peek);
        return parse_json_array(lexer);
    } else {
        // string, número, true, false, null
        Token_build_t* tok = lexer_next_token(lexer, token_analysis);
        ast_node_t* node = create_ast_node_t(strdup((const char*)tok->value_process));
        free(tok);
        free((void*)peek);
        return node;
    }
}

// --- Parseo de objetos JSON ---
ast_node_t* parse_json_object(Lexer_t* lexer) {
    Token_build_t* tok = lexer_next_token(lexer, token_analysis); // consume '{'
    ast_node_t* obj_node = create_ast_node_t(strdup("{"));

    while (1) {
        const char* peek = peek_token_str(lexer);
        if (strcmp(peek, "}") == 0) {
            free((void*)peek);
            break;
        }
        // clave
        Token_build_t* key_tok = lexer_next_token(lexer, token_analysis);
        ast_node_t* key_node = create_ast_node_t(strdup((const char*)key_tok->value_process));
        free(key_tok);

        // :
        Token_build_t* colon_tok = lexer_next_token(lexer, token_analysis);
        ast_node_t* colon_node = create_ast_node_t(strdup((const char*)colon_tok->token->name_token));
        free(colon_tok);

        // valor
        ast_node_t* value_node = parse_json_value(lexer);

        // Estructura: key -> : -> value
        push_back_a(key_node->ramas, colon_node);
        push_back_a(colon_node->ramas, value_node);

        // Agregar al objeto
        push_back_a(obj_node->ramas, key_node);

        // , o }
        peek = peek_token_str(lexer);
        if (strcmp(peek, ",") == 0) {
            Token_build_t* comma_tok = lexer_next_token(lexer, token_analysis);
            free(comma_tok);
        } else if (strcmp(peek, "}") == 0) {
            free((void*)peek);
            break;
        } else {
            printf("Error de parseo: se esperaba ',' o '}'\n");
            free((void*)peek);
            break;
        }
    }
    Token_build_t* end_tok = lexer_next_token(lexer, token_analysis); // consume '}'
    free(end_tok);
    return obj_node;
}

// --- Parseo de arrays JSON ---
ast_node_t* parse_json_array(Lexer_t* lexer) {
    Token_build_t* tok = lexer_next_token(lexer, token_analysis); // consume '['
    ast_node_t* arr_node = create_ast_node_t(strdup("["));

    while (1) {
        const char* peek = peek_token_str(lexer);
        if (strcmp(peek, "]") == 0) {
            free((void*)peek);
            break;
        }
        ast_node_t* value_node = parse_json_value(lexer);
        push_back_a(arr_node->ramas, value_node);

        peek = peek_token_str(lexer);
        if (strcmp(peek, ",") == 0) {
            Token_build_t* comma_tok = lexer_next_token(lexer, token_analysis);
            free(comma_tok);
        } else if (strcmp(peek, "]") == 0) {
            free((void*)peek);
            break;
        } else {
            printf("Error de parseo: se esperaba ',' o ']'\n");
            free((void*)peek);
            break;
        }
    }
    Token_build_t* end_tok = lexer_next_token(lexer, token_analysis); // consume ']'
    free(end_tok);
    return arr_node;
}

// Busca el valor de una clave en un objeto JSON AST
ast_node_t* get_json_value_by_key(ast_node_t* obj, const char* key) {
    if (!obj || !obj->ramas) return NULL;
    size_t n = size_a(obj->ramas);
    for (size_t i = 0; i < n; ++i) {
        ast_node_t* key_node = (ast_node_t*)get_element_a(obj->ramas, i);
        if (!key_node || !key_node->data) continue;
        if (strcmp((char*)key_node->data, key) == 0) {
            // El valor está en key_node->ramas[0]->ramas[0]
            if (size_a(key_node->ramas) > 0) {
                ast_node_t* colon_node = (ast_node_t*)get_element_a(key_node->ramas, 0);
                if (size_a(colon_node->ramas) > 0) {
                    return (ast_node_t*)get_element_a(colon_node->ramas, 0);
                }
            }
        }
    }
    return NULL;
}


// Obtiene el nodo del array en la posición idx
ast_node_t* get_json_array_element(ast_node_t* array, size_t idx) {
    if (!array || !array->ramas) return NULL;
    if (idx >= size_a(array->ramas)) return NULL;
    return (ast_node_t*)get_element_a(array->ramas, idx);
}

// Busca un valor por ruta tipo "address/city"
ast_node_t* get_json_value_by_path(ast_node_t* node, const char* path) {
    char* path_copy = strdup(path);
    char* token = strtok(path_copy, "/");
    ast_node_t* current = node;
    while (token && current) {
        current = get_json_value_by_key(current, token);
        token = strtok(NULL, "/");
    }
    free(path_copy);
    return current;
}

int main() {
#ifdef _WIN32
    #include <windows.h>
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // Cargar archivo JSON
    FILE* f = fopen("ejemplo.json", "rb");
    if (f == NULL) {
        puts("ejemplo.json no encontrado");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    char* datos = malloc(sz+1);
    fread(datos, 1, sz, f);
    datos[sz] = 0;
    fclose(f);

    Lexer_t lexer = init_lexer(datos, sz);

    // --- REGISTRO DE TOKENS ---
    push_token(&lexer, create_token("{", "{", token_llave_init));
    push_token(&lexer, create_token("}", "}", token_llave_end));
    push_token(&lexer, create_token(",", ",", token_coma));
    push_token(&lexer, create_token(":", ":", token_doble_punto));
    push_token(&lexer, create_token("[", "[", token_corchete_init));
    push_token(&lexer, create_token("]", "]", token_corchete_end));
    push_token(&lexer, create_token("true", "true", token_true));
    push_token(&lexer, create_token("false", "false", token_false));
    push_token(&lexer, create_token("null", "null", token_null));
    push_token(&lexer, create_token(build_token_special(TOKEN_NUMBER), build_token_special(TOKEN_NUMBER), inc_token()));
    push_token(&lexer, create_token(build_token_special(TOKEN_STRING_SIMPLE), build_token_special(TOKEN_STRING_SIMPLE), inc_token()));
    push_token(&lexer, create_token(build_token_special(TOKEN_STRING_DOUBLE), build_token_special(TOKEN_STRING_DOUBLE), inc_token()));
    push_token(&lexer, create_token(build_token_special(TOKEN_EOF), build_token_special(TOKEN_EOF), inc_token()));
    push_token(&lexer, create_token(build_token_special(TOKEN_ID), build_token_special(TOKEN_ID), inc_token()));

    build_lexer(&lexer);

    // --- Parsear el JSON ---
    ast_node_t* json_ast = parse_json_value(&lexer);

    printf("AST del archivo JSON:\n");
    print_ast(json_ast, inorder_ast_with_ascii);

    // --- Buscar nodos por valor ---
    printf("Buscando todos los nodos con valor \"Juan\":\n");
    find_nodes_by_value(json_ast, "name");

    ast_node_t* name_value = get_json_value_by_key(json_ast, "name");
    if (name_value) printf("name: %s\n", (char*)name_value->data);


    ast_node_t* grades_node = get_json_value_by_key(json_ast, "grades");
    if (grades_node) {
        ast_node_t* first_grade = get_json_array_element(grades_node, 0);
        if (first_grade) printf("Primer grade: %s\n", (char*)first_grade->data);
    }

    ast_node_t* city_node = get_json_value_by_path(json_ast, "address/city");
    if (city_node) printf("city: %s\n", (char*)city_node->data);


    ast_node_t* projects_node = get_json_value_by_key(json_ast, "projects");
    if (projects_node) {
        for (size_t i = 0; i < size_a(projects_node->ramas); ++i) {
            ast_node_t* project = (ast_node_t*)get_element_a(projects_node->ramas, i);
            // Cada project es un objeto, puedes acceder a sus claves:
            ast_node_t* title_node = get_json_value_by_key(project, "title");
            ast_node_t* score_node = get_json_value_by_key(project, "score");
            if (title_node && score_node)
                printf("Project %zu: %s (%s)\n", i, (char*)title_node->data, (char*)score_node->data);
        }
    }

    if (projects_node) {
        for (size_t i = 0; i < size_a(projects_node->ramas); ++i) {
            ast_node_t* project = (ast_node_t*)get_element_a(projects_node->ramas, i);
            ast_node_t* title_node = get_json_value_by_key(project, "title");
            if (title_node) printf("Proyecto %zu: %s\n", i, (char*)title_node->data);
        }
    }


    // --- Liberar memoria ---
    free_ast_t(json_ast, free); // Si usas strdup, pasa free
    free_lexer(&lexer);
    free(datos);

    puts("exit");
    return 0;
}
