#include "include/parser.h"

typedef enum names_tokens {
    token_llave_init,   // {
    token_llave_end,    // }
    token_coma,         // ,
    token_doble_punto,  // :
    token_punto,        // .
    token_corch_init,   // [
    token_corch_end,    // ]
    token_number_float,
} names_tokens;

const Token_id inc_token(void) {
    static Token_id my_token_id = token_number_float+1;
    return my_token_id++;
}

Token_build_t* lexer_parser_number_float(Lexer_t *lexer, Token_build_t* n1, Token_build_t* n2) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , lexer_parser_number_float)
            TYPE_DATA_DBG(Token_build_t*, "n1 = %p")
            TYPE_DATA_DBG(Token_build_t*, "n2 = %p")
        END_TYPE_FUNC_DBG,
        n1, n2);
    Token_build_t* self = init_token_build(NULL); 

    size_t size_n1 = strlen(n1->value_process), size_n2 = strlen(n2->value_process);
    debug_calloc(char, self->value_process, size_n1 + size_n2 + 1 +1, sizeof(char)); // +1 para el terminador '\0' y +1 para el caracter del punto

    // concatenar "num" + "." + "num"
    strcat( self->value_process, n1->value_process);
    strcat( self->value_process, ".");
    strcat( self->value_process, n2->value_process);

    self->token = (Token_t *)get(lexer->hash_table, build_token_special(TOKEN_NUMBER_FLOAT));
    if (self->token == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Token no encontrado\n");
        return NULL;
    }

    // reservar memoria para un flotante
    //self->value_process = malloc(sizeof(float));
    //*((float*)self->value_process) = (float)atof(n1->value_process) + (float)atof(n1->value_process);
    
    return self;
}


Token_build_t* token_analysis (Lexer_t *lexer) {
    /*
        Los elementos de tipo `Token_build_t*` devueltos por esta funcion deben ser liberados
        con free por parte del programador.
     */
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , token_analysis)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        lexer);
    Token_build_t* self;

    repeat_switch:
        if (isdigit(lexer->chartter)) {
            // numero o parte entera:
            self = lexer_parser_number(lexer);

            /*
                si el caracter actual es un punto y el siguiente caracter es un numero
                se trata de un numero decimal
            */
            if (lexer->chartter == '.' && isdigit(lexer->data[lexer->index + 1])) {
                lexer_advance(lexer); // saltar el punto

                // crear un token compuesto de tipo float con dos tokens int:
                return lexer_parser_number_float(lexer, self, lexer_parser_number(lexer));
            } else return self;
        }

        switch (lexer->chartter)
        {
        case 0:
            self = init_token_build(NULL);
            self->token = get(lexer->hash_table, build_token_special(TOKEN_EOF));
            return self;
        case '"':
        case '\'':
            return lexer_parser_string(lexer);
        case '{':
        case '}':
        case ':':
        case ',':
        case '.':
        case '[':
        case ']':
            self = init_token_build((Object)&(lexer->chartter));
            self->token = get_token(lexer, (const char[]){lexer->chartter, 0});
            lexer_advance(lexer);
            return self;
        case '\n':
        case ' ':
            lexer_advance(lexer);
            goto repeat_switch;
        default:
            printf_color("no definido %d %c\n", lexer->chartter, lexer->chartter);
            return NULL; 
        }

}

ASTNode* calc_first_level(Ast_t *ast, Token_build_t *tok) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(ASTNode*  , calc_first_level)
            TYPE_DATA_DBG(Token_build_t*, "tok = %p")
            TYPE_DATA_DBG(Ast_t*, "ast = %p")
        END_TYPE_FUNC_DBG,
        tok,ast);
   /*
     * en una expresion json `"nombre" : 12,`  el string nombre es un token de primer nivel,
     * el ":" un token de segundo nivel y el 12 un token de tercer nivel, mientras que la coma es de 4to nivel
     *
     * Una expresion como:
     * 12 : {
     *     "nombre" : 12,
     *     "edad" : 23
     * }
     *
     * 12 es un token de primer nivel, el token : es de segundo nivel, el token `{"nombre" : 12,...}` es de tercer nivel.
     * El diccionario `{"nombre" : 12,...}` debera ser revaluado como una nueva sub-expresion que forma parte de la expresion
     * padre. por tanto a nivel de subexpresion, el token "nombre" es de primer nivel, el token : es de segundo nivel,
     * el token 12 es de tercer nivel y la coma es de cuarto nivel. "edad" es de primer nivel, el token : es de segundo nivel,
     * el token 23 es de tercer nivel y la coma es de cuarto nivel.
     */

    // Iteramos sobre cada árbol en la raíz del AST para ver si en el primer nivel coincide el token
    // si no se encontrara, entonces se encontro un error de sintaxis
    size_t numTrees = size_v(ast->root);
    ASTNode* tree   = NULL;
    for (size_t i = 0; i < numTrees; i++) {
        ASTNode* existingTree = get_element_v(ast->root, i);
        DEBUG_PRINT(DEBUG_LEVEL_INFO, "existingTree->value %d\n", existingTree->value);
        if (existingTree->ramas == NULL || size_v(existingTree->ramas) == 0) {
            // Si no hay ramas, continuar con el siguiente árbol
            continue;
        }
        tree = (ASTNode*)get_element_v(existingTree->ramas, 0);
        if (existingTree->value == tok->token->type) {
            // se encontro el token de primer nivel
            //goto item_found;
            return tree;
        }
    }
    DEBUG_PRINT(DEBUG_LEVEL_INFO, "Error de sintaxis?: Se esperaba un (token:%d) se encontro un (token:%d)'%c'\n", tree->value,tok->token->type,tok->token->type);
    return NULL; // si el bucle finalizo sin saltar, no se encontro el token de primer nivel y por tanto,
    // se encontro un error de sintaxis

    //item_found: // si se encontro en el primer nivel, se continua con la busqueda en los subniveles
}

ASTNode* calc_nexts_levels(ASTNode* ast_branch, Lexer_t* lexer) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(ASTNode*  , calc_nexts_levels)
            TYPE_DATA_DBG(ASTNode*, "ast_branch = %p")
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        ast_branch,lexer);
    if (ast_branch == NULL) {
        return NULL; // Caso base: No hay más nodos por procesar
    }

    // Obtener el siguiente token del lexer
    Token_build_t* tok = lexer_next_token(lexer, token_analysis);
    if (!tok) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: No se pudo obtener el siguiente token\n");
        return NULL;
    }
    // Si se encuentra un token_coma, señalamos división de línea
    if (tok->token->type == token_coma) {
        DEBUG_PRINT(DEBUG_LEVEL_INFO, "Token coma encontrado. Finalizando análisis de línea actual.\n");
        free(tok);
        return NULL; // Finaliza esta línea, un token coma fue encontrado, hora de reiniciar la busqueda
    }

    // Verificar si el token coincide con el nodo actual
    if (ast_branch->value == tok->token->type) {
        DEBUG_PRINT(DEBUG_LEVEL_INFO, "Coincidencia encontrada en el nivel: %d\n", ast_branch->value);
        return ast_branch; // Coincidencia encontrada
    }

    // Recorrer todas las ramas del nodo actual
    size_t numBranches = size_v(ast_branch->ramas);
    for (size_t i = 0; i < numBranches; i++) {
        ASTNode* subnode = (ASTNode*)get_element_v(ast_branch->ramas, i);
        ASTNode* result = calc_nexts_levels(subnode, lexer); // Llamada recursiva
        if (result != NULL) {
            DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Coincidencia encontrada en un subnivel\n", tok->token->type);
            return result; // Coincidencia encontrada en un subnivel
        }
    }

    // Si no se encontró una coincidencia en este nivel o subniveles
    DEBUG_PRINT(DEBUG_LEVEL_WARNING, "Error de sintaxis: No se encontró coincidencia para el token token(%d) en este nivel\n", tok->token->type);
    return NULL;
}


int calc_ast_vals(Ast_t *ast, Lexer_t *lexer) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(int  , calc_ast_vals)
            TYPE_DATA_DBG(Ast_t*, "ast = %p")
            TYPE_DATA_DBG(Token_build_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        ast, lexer);
    // restaurar el lexer, es necesario para poder seguir operando con el si alguien o algo lo modifico
    //restore_lexer(lexer);

    int val = 0;
    LinkedList* list_expr_sintax = createLinkedList();

    // obtener el ID de los tokens de tipo numero:
    Token_id token_number = ((Token_t *)get(lexer->hash_table, build_token_special(TOKEN_NUMBER)))->type;
    Token_id token_eof    = ((Token_t*)get(lexer->hash_table, build_token_special(TOKEN_EOF)))->type;

    int operador = 0;

    // obtener el primer token
    Token_build_t* tok = lexer_next_token(lexer, token_analysis);

    /*
     * Token[token_llave_init]: "{", Type: 0
     * Token[token_llave_end]: "}", Type: 1
     * Token[token_coma]: ",", Type: 2
     * Token[token_doble_punto]: ":", Type: 3
     * Token[token_punto]: ".", Type: 4
     * Token[token_corch_init]: "[", Type: 5
     * Token[token_corch_end]: "]", Type: 6
     * Token[token_number_float]: "?{TOKEN_NUMBER_FLOAT}", Type: 7
     * Token[token_number]: "?{TOKEN_NUMBER}", Type: 8
     * Token[token_id]: "?{TOKEN_ID}", Type: 9
     * Token[token_eof]: "?{TOKEN_EOF}", Type: 10
     * Token[token_string_simple]: "?{TOKEN_STRING_SIMPLE}", Type: 11
     * Token[token_string_double]: "?{TOKEN_STRING_DOUBLE}", Type: 12
     * AST Structure:
     * └── 12
     *     └── 3
     *         ├── 12
     *         ├── 8
     *         ├── 5
     *         ├── 0
     *         └── 7
     * └── 8
     *     └── 3
     *         ├── 12
     *         ├── 8
     *         ├── 5
     *         ├── 0
     *         └── 7
     * └── 7
     *     └── 3
     *         ├── 12
     *         ├── 7
     *         ├── 8
     *         ├── 5
     *         └── 0
     */

    if (tok->token->type != token_llave_init) {
        // sintaxis error, se esperaba un '{'
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error de sintaxis: Se esperaba un '{', pero se encontre un token(%d)\n", tok->token->type);
        return -1;
    }

    // obtener el segundo token, su type debe ser, 12, 8 o 7
    ASTNode *ast_branch;
    // mientras no se llegue al final de los tokens
    continue_parsing:
    for (int i = 0; (tok = lexer_next_token(lexer, token_analysis))->token->type != token_eof; i++) {
        // para el primer elemento hacer una busqueda binaria en el primer nivel del ast multi-nivel y multi-rama
        if (((ast_branch = calc_first_level(ast, tok))) == NULL) {
            if (tok->token->type == token_llave_end) {
                return 0; // finalizar el analisis, la sintaxis es correcta
            } else if (tok->token->type == token_llave_init) {
                // retroceder una posicion en el lexer
                lexer->index--;
                lexer->chartter = lexer->data[lexer->index];
                return calc_ast_vals(ast, lexer); // se encontro un '{', un subdiccionario
            }
            DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error de sintaxis: Se esperaba un '{' o un '}' %d\n", tok->token->type);
            return -1;
        }
        print_token(tok->token);
        printf_color("token: %s\n", tok->token->name_token);
        
        if (calc_nexts_levels(ast_branch,  lexer) != NULL) {
            // si se encontro una coincidencia en el primer nivel
            DEBUG_PRINT(DEBUG_LEVEL_INFO, "Coincidencia encontrada en el primer nivel\n");
        }
        
        free(tok);
    }
    free(tok); // liberar el Token_build_t
    restore_lexer(lexer);
    return val;
}


int main() {
    #ifdef _WIN32
        #include <windows.h>
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    const char datos[] =
    "{\n"
    "    \"version\": 3.0,\n"
    "\n"
    "    \"doc\": \"programa para descargar videos y musica\",\n"
    "\n"
    "    \"autor\": {\n"
    "        \"usuario\": \"Mario\",\n"
    "        \"github\": \"https://github.com/Maalfer/\"\n"
    "    },\n"
    "\n"
    "    \"contribuidores\": [{\n"
    "            \"usuario\": \"desmon\",\n"
    "            \"github\": \"https://github.com/desmonHak/\"\n"
    "        },\n"
    "        {\n"
    "            \"usuario\": \"Mario\",\n"
    "            \"github\": \"https://github.com/Maalfer/\"\n"
    "        }\n"
    "    ]\n"
    "}\n"
    ;
    Lexer_t lexer = init_lexer(datos, sizeof(datos));

    func_auto_increment func = inc_token;

    const position positions_tokens[] = {
        push_token(&lexer, create_token(create_name_token(token_llave_init),    "{",                                      token_llave_init)),
        push_token(&lexer, create_token(create_name_token(token_llave_end),     "}",                                       token_llave_end)),
        push_token(&lexer, create_token(create_name_token(token_coma),          ",",                                             token_coma)),
        push_token(&lexer, create_token(create_name_token(token_doble_punto),   ":",                                      token_doble_punto)),
        push_token(&lexer, create_token(create_name_token(token_punto),         ".",                                            token_punto)),
        push_token(&lexer, create_token(create_name_token(token_corch_init),    "[",                                       token_corch_init)),
        push_token(&lexer, create_token(create_name_token(token_corch_end),     "]",                                        token_corch_end)),

        // los tokens compuestos como number float(se forman de dos tokens number), son tokens especiales
        push_token(&lexer, create_token(create_name_token(token_number_float), build_token_special(TOKEN_NUMBER_FLOAT),  token_number_float)),

        push_token(&lexer, create_token(create_name_token(token_number),        build_token_special(TOKEN_NUMBER),                inc_token)),
        push_token(&lexer, create_token(create_name_token(token_id),            build_token_special(TOKEN_ID),                    inc_token)),
        push_token(&lexer, create_token(create_name_token(token_eof),           build_token_special(TOKEN_EOF),                   inc_token)),
        push_token(&lexer, create_token(create_name_token(token_string_simple), build_token_special(TOKEN_STRING_SIMPLE),         inc_token)),
        push_token(&lexer, create_token(create_name_token(token_string_double), build_token_special(TOKEN_STRING_DOUBLE),         inc_token))
    };

    //print_tokens(&lexer);

    // construir el lexer con los tokens
    build_lexer(&lexer);

    print_Token_build(&lexer, token_analysis);
    
    Ast_t *ast = init_ast(&lexer);

    const char* expressions[][5] = {
        {build_token_special(TOKEN_STRING_DOUBLE),  ":", build_token_special(TOKEN_STRING_DOUBLE), ",", NULL}, // "string"   :   "string"

        {build_token_special(TOKEN_NUMBER),         ":", build_token_special(TOKEN_STRING_DOUBLE), ",", NULL}, // 123        :   "string"
        {build_token_special(TOKEN_NUMBER),         ":", build_token_special(TOKEN_NUMBER), ",", NULL},        // 123        :   123
        {build_token_special(TOKEN_STRING_DOUBLE),  ":", build_token_special(TOKEN_NUMBER), ",", NULL},        // "string"   :   123

        {build_token_special(TOKEN_STRING_DOUBLE),  ":", "[", ",", NULL},                                      // "string"   :   [
        {build_token_special(TOKEN_NUMBER),         ":", "[", ",", NULL},                                      // 123        :   [

        {build_token_special(TOKEN_STRING_DOUBLE),  ":", "{", ",", NULL},                                      // "string"   :   {
        {build_token_special(TOKEN_NUMBER),         ":", "{", ",", NULL},                                      // 123        :   {
        

        {build_token_special(TOKEN_NUMBER_FLOAT),   ":", build_token_special(TOKEN_STRING_DOUBLE), ",", NULL}, // 1.0        :   "string"
        {build_token_special(TOKEN_STRING_DOUBLE),  ":", build_token_special(TOKEN_NUMBER_FLOAT), ",", NULL},  // "string"   :   1.0
 
        {build_token_special(TOKEN_NUMBER),         ":", build_token_special(TOKEN_NUMBER_FLOAT), ",", NULL},  // 123        :   1.0
        {build_token_special(TOKEN_NUMBER_FLOAT),   ":", build_token_special(TOKEN_NUMBER_FLOAT), ",", NULL},  // 1.0        :   1.0
        {build_token_special(TOKEN_NUMBER_FLOAT),   ":", build_token_special(TOKEN_NUMBER), ",", NULL},        // 1.0        :   123
 
        {build_token_special(TOKEN_NUMBER_FLOAT),   ":", "[", ",", NULL},                                      // 1.0        :   [
        {build_token_special(TOKEN_NUMBER_FLOAT),   ":", "{", ",", NULL},                                      // 1.0        :   {

    };

    // Itera sobre el array y crea las expresiones
    for (size_t i = 0; i < sizeof(expressions) / sizeof(expressions[0]); i++) {
        add_expression_to_ast(ast, create_expression(ast, expressions[i][0], expressions[i][1], expressions[i][2], expressions[i][3], expressions[i][4]));
    }

    print_tokens(&lexer);
    print_ast(ast);

    printf("valor de la operacion: %d\n", calc_ast_vals(ast, &lexer));

    int sequence[] = {4, 3, 8};
    size_t seq_size = sizeof(sequence) / sizeof(sequence[0]);

    if (is_sequence_in_ast(ast, sequence, seq_size)) {
        printf("La secuencia está presente en el AST.\n");
    } else {
        printf("La secuencia no está presente en el AST.\n");
    }

    // Liberar la memoria del AST y el Lexer
    freeAst(ast);
    free_lexer(&lexer);


    puts("exit");
    puts(datos);
    return 0;
}