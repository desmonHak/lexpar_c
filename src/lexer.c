#ifndef LEXPAR_LEXER_C
#define LEXPAR_LEXER_C

#include "global.h"

Lexer_t init_lexer(const char* data, uint64_t size) {
    unsigned char c;
    if (data == NULL) c = '\0';
    else c = data[0];

    return (Lexer_t){
        .list_id_tokens = createArrayList(0, NULL),
        .hash_table     = NULL,
        .chartter       = c,
        .index          = 0,
        .data           = data,
        .size           = size
    };
}

/*
Permite añadir un token a la lista de tokens
*/
position push_token(Lexer_t *lexer, Token_t *token) {
    push_back_a(lexer->list_id_tokens, token);
    return lexer->size;
}

/*
avanzar el lexer una posicion
 */
void lexer_advance(Lexer_t* lexer){
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(void  , lexer_advance)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        lexer);
    if (lexer->index + 1 >= lexer->size) {
        lexer->chartter = '\0'; // Final de los datos
        return;
    }
    if (lexer->index < lexer->size && lexer->chartter != '\0'){
        lexer->index++;
        lexer->chartter = lexer->data[lexer->index];
    }
}

Token_build_t* lexer_parser_id(Lexer_t* lexer){
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , lexer_parser_id)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        lexer);
    unsigned char * value = (unsigned char*)calloc(1, sizeof(unsigned char));

    while (isalnum(lexer->chartter) || lexer->chartter == '_' || lexer->chartter == '-')
    {
        value = (unsigned char*)realloc(value, (strlen(value) + 2) * sizeof(unsigned char));
        if (value == NULL) {
            free(value);
            DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: no se pudo asignar memoria\n");
            return NULL; // O maneja el error apropiadamente
        }

        strcat(value, (char[]){lexer->chartter, 0});

        // siempre que el siguiente caracter sea una letra o un dijito, ejecutara lexer_advance, sino interumpe el bucle
        if (isalnum(lexer->data[lexer->index+1]) ||  lexer->data[lexer->index+1] == '_' || lexer->data[lexer->index+1] == '-')
            lexer_advance(lexer);
        else break;
    }
    Token_build_t* self = init_token_build(value);
    self->token = get(lexer->hash_table, build_token_special(TOKEN_ID));
    if (self->token == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Token no encontrado (self->token)\n");
        return NULL;
    }
    return self;
}


long long strtoll_plus(const char *str, char **endptr, int base) {
    errno = 0;
    
    if (strncmp(str, "0b", 2) == 0 || strncmp(str, "0B", 2) == 0) {
        return strtoll(str + 2, endptr, 2);  // Base 2, omite "0b"
    }

    // Si no es binario, usa base automática (0 = detecta base por prefijo)
    return strtoll(str, endptr, base);
}

Token_build_t* lexer_parser_number(Lexer_t* lexer){
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , lexer_parser_number)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        lexer);
    /*
     *
     *  Esta funcion convierte un valor en entero asignando un token de tipo int
     *  permite detectar si es un valor hexadecimal, binario o decimal tmb
     *
     */

    unsigned char * value;// = (unsigned char*)calloc(1, sizeof(unsigned char));
    
    if (lexer->chartter == '-') {
        debug_calloc(unsigned char, value, 2, sizeof(unsigned char));
        value[0] = lexer->chartter;
        lexer_advance(lexer);
    } else debug_calloc(unsigned char, value, 1, sizeof(unsigned char));

    while (
        isdigit(lexer->chartter) || // para digitos
        lexer->chartter == 'x'   || // para hexadecimal
        lexer->chartter == 'a'   || // para hexadecimal
        lexer->chartter == 'b'   || // para hexadecimal o binario
        lexer->chartter == 'c'   || // para hexadecimal
        lexer->chartter == 'd'   || // para hexadecimal
        lexer->chartter == 'e'   || // para hexadecimal
        lexer->chartter == 'f'      // para hexadecimal
    )
    {
        /*
         *
         *  Si es un digito decimal(todos son numeros ),
         *  O un valor hexadimal (hay un caracter x de por medio),
         *  O un valor binario (hay un caracter b de por medio),
         *  Se considera como entero
         * 
         */
        value = (unsigned char*)realloc(value, (strlen(value) + 2) * sizeof(unsigned char));
        if (value == NULL) {
            free(value);
            DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: no se pudo asignar memoria\n");
            return NULL; // O maneja el error apropiadamente
        }
        strcat(value, (char[]){lexer->chartter, 0});
        lexer_advance(lexer);
    }
    /*
    inicializar un token_build con el valor obtenido y asociandolo con su token
    */
    Token_build_t* self = init_token_build(value);
    self->token = get(lexer->hash_table, build_token_special(TOKEN_NUMBER));
    if (self->token == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Token no encontrado (self->token == NULL)\n");
        return NULL;
    }
    return self;
}

void lexer_skip_whitespace(Lexer_t* lexer){
    // si la posicion antual es alguno de estos caracteres " \t\r\n"
    // saltar al siguiente caracter
    while (lexer->chartter == '\r' || lexer->chartter == '\t') lexer_advance(lexer);
}

Token_build_t* lexer_advance_with(Lexer_t* lexer, Token_build_t* token){
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , lexer_advance_with)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
            TYPE_DATA_DBG(Token_build_t*, "token = %p")
        END_TYPE_FUNC_DBG,
        lexer, token);
    lexer_advance(lexer);
    return token;
}

void print_tokens(Lexer_t *lexer) {
    if (lexer == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "#{FG:red}Error: #{FG:white}lexer no es valido (lexer == NULL)\n");
        return;
    }

    if (lexer->list_id_tokens == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "#{FG:red}Error: #{FG:white}La lista de tokens no fue inicializada (lexer->list_id_tokens == NULL)\n");
        return;
    }
    forEachNew(lexer->list_id_tokens, (void(*)(void *))print_token);
}

void build_lexer(Lexer_t *lexer) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(void  , build_lexer)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
        END_TYPE_FUNC_DBG,
        lexer);
    const position size_list_tokens = size(lexer->list_id_tokens);
    if (size_list_tokens == 0){
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "#{FG:red}Error: #{FG:white}No se han encontrado tokens\n");
        return;
    }
    // hacer una tabla de hash para todos los tokens, con el tamaño de la lista de tokens
    lexer->hash_table = createHashTable(size_list_tokens);

    // tokens predefinidos
    token_number  = get(lexer->hash_table, build_token_special(TOKEN_NUMBER));
    token_eof     = get(lexer->hash_table, build_token_special(TOKEN_EOF));
    token_id      = get(lexer->hash_table, build_token_special(TOKEN_ID));

    for (position i = 0; i < size_list_tokens; i++) {
        // curent = list_id_tokens[i]
        Token_t*    current = lexer->list_id_tokens->Array[i];
        if (current == NULL) {
            DEBUG_PRINT(DEBUG_LEVEL_ERROR, "#{FG:red}Error: #{FG:white}No se ha podido obtener el token %d\n", i);
            continue;
        }
        // añadir a la tabla de hash cada uno de los tokens con sus valores
        put(lexer->hash_table, current->value, current);
    }
}

Token_t *get_token(Lexer_t *lexer, const char *value) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_t *, print_Token_build)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
            TYPE_DATA_DBG(func_token_analysis, "value = %s")
        END_TYPE_FUNC_DBG,
        lexer, value);
    Token_t * token = get(lexer->hash_table, value);
    if (token == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Token no encontrado (self->token == NULL)\n");
        return nullptr;
    }
    return token;
}

// Función para imprimir la información de un Token_build_t
void printTokenBuildInfo(const Token_build_t* token_build) {
    if (token_build == NULL) {
        printf_color("Token_build_t es #{FG:lred}(NULL)#{reset}.\n");
        return;
    }

    // Verificar si el token asociado es válido
    if (token_build->token == NULL) {
        printf_color("Token_build_t contiene un token #{FG:lred}(NULL)#{reset}.\n");
        return;
    }

    const Token_t* token = token_build->token;

    // Imprimir informacion del token
    printf_color("Informacion del Token_build_t:\n");
    printf_color("--------------------------------\n");
    printf_color("Nombre del Token: #{FG:lgreen}%s#{reset}\n", token->name_token ? token->name_token : "(#{FG:lred}NULL#{reset})");
    printf_color("Tipo del Token (ID): %u\n", token->type);

    // Imprimir el valor del token si está disponible
    if (token->value) {
        printf_color("Valor del Token: #{FG:lcyan}%s#{reset}\n", token->value);
    } else {
        printf_color("Valor del Token: (#{FG:lred}(NULL)#{reset})\n");
    }

    // Imprimir el valor procesado si está disponible
    if (token_build->value_process) {
        printf_color("Valor Procesado: #{FG:purple}%s#{reset}\n", token_build->value_process);
    } else {
        printf_color("Valor Procesado: (#{FG:lred}(NULL)#{reset})\n");
    }

    printf_color("--------------------------------\n");
}
void print_Token_build(Lexer_t* lexer, func_token_analysis token_analysis){
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(void  , print_Token_build)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
            TYPE_DATA_DBG(func_token_analysis, "token_analysis = %p")
        END_TYPE_FUNC_DBG,
        lexer, token_analysis);
    if (lexer == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Lexer no inicializado\n");
        return;
    }
    if (token_analysis == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Token analysis no inicializado\n");
        return;
    }
    Token_build_t* tok;
    
    Token_id token_eof    = ((Token_t*)get(lexer->hash_table, build_token_special(TOKEN_EOF)))->type;
    while ( 1 ) {
        tok = lexer_next_token(lexer, token_analysis);
        if ( tok == NULL ) {
            DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Tok devolvio NULL (tok == NULL)\n");
        } else {
            if ( tok->token == NULL ) {
                DEBUG_PRINT(DEBUG_LEVEL_ERROR, "Error: Token no encontrado (tok->token == NULL)\n");
                goto exit_free_tok;
            } else { // imprimir el token
                if ( token_eof == tok->token->type ) break;
                print_token(tok->token);
                printf_color("\tVal token: %s\n", (const char *)tok->value_process);
                free(tok);
            }
        }
    }

    print_token(tok->token);
    printf_color("\tVal token: %p\n", tok->value_process);

    exit_free_tok:
    if (tok != NULL) free(tok); // liberar el Token_build_t

    restore_lexer:
    // restaurar el lexer, es necesario para poder seguir operando con el
    restore_lexer(lexer);
}

void restore_lexer(Lexer_t* lexer) {
    lexer->index = 0;
    lexer->chartter = lexer->data[lexer->index];
}

Token_build_t* lexer_parser_string(Lexer_t* lexer){
    Token_t * type_token = NULL;
    unsigned char * value = (unsigned char*)calloc(1, sizeof(unsigned char));
    unsigned char caracter_prohibido = '"';
    /*if ( 
        lexer->src[lexer->i]   == '"' && 
        lexer->src[lexer->i+1] == '"' && 
        lexer->src[lexer->i+2] == '"'    
    ) {
        type_token = TOKEN_DOC_STRING;
        caracter_prohibido = '"';
        lexer_advance(lexer);lexer_advance(lexer);
    } else if (
        lexer->src[lexer->i]   == '\'' && 
        lexer->src[lexer->i+1] == '\'' && 
        lexer->src[lexer->i+2] == '\''   
    ) { 
        type_token = TOKEN_DOC_STRING;
        caracter_prohibido = '\'';
        lexer_advance(lexer);lexer_advance(lexer);
    } 
    else*/ if (
        lexer->data[lexer->index]   == '"'
    ) {
        type_token = get(lexer->hash_table, build_token_special(TOKEN_STRING_DOUBLE));
        caracter_prohibido = '"';
    } else if (lexer->data[lexer->index]   == '\'') {
        type_token = get(lexer->hash_table, build_token_special(TOKEN_STRING_SIMPLE));
        caracter_prohibido = '\'';
    } else {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "#{FG:red}Error: #{FG:white}No se ha podido obtener el token\n");
        return NULL;
    }
    lexer_advance(lexer);

    while (1){
        // si se encontro una \ y no se encontr el caracter prohibido o uno de escape
        // se finaliza el bucle 
        //printf(" last(%c) %hu = %c -> caracter_prohibido  %hu = %c \n", lexer->src[lexer->i-1], lexer->c, lexer->c, caracter_prohibido, caracter_prohibido );
        if (lexer->chartter == caracter_prohibido && lexer->data[lexer->index-1] != '\\') break;
        /*else if (lexer->c == 0x0){
            puts("ERROR  lexer");
            exit(1);
        }*/
        value = (unsigned char*)realloc(value, (strlen(value) + 2) * sizeof(unsigned char));
        strcat(value, (char[]){lexer->chartter, 0});
        lexer_advance(lexer);
        DEBUG_PRINT(DEBUG_LEVEL_INFO, "%p\n", lexer->index);
    }
    
    lexer_advance(lexer);
    Token_build_t*self = init_token_build(value);
    self->token = type_token;
    if (self->token == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "#{FG:red}Error: #{FG:white}No se ha podido obtener el token EOF\n");
        return NULL;
    }
    return self;
}

/**
 * Los elementos de tipo `Token_build_t*` devueltos por esta funcion deben ser liberados
 *         con free por parte del programador.
 * @param lexer lexer en del que devolver un token.
 * @param token_analysis funcion para analizar tokens
 * @return se devuelve el token en cuestion.
 */
Token_build_t* lexer_next_token(Lexer_t* lexer, func_token_analysis token_analysis) {
    DEBUG_PRINT(DEBUG_LEVEL_INFO,
        INIT_TYPE_FUNC_DBG(Token_build_t*  , lexer_next_token)
            TYPE_DATA_DBG(Lexer_t*, "lexer = %p")
            TYPE_DATA_DBG(func_token_analysis, "token_analysis = %p")
        END_TYPE_FUNC_DBG,
        lexer, token_analysis);
    //print_token(token_number);
    //print_token(token_eof);
    //print_token(token_id);

    Token_build_t *self;
    while (lexer->chartter != '\0') {

        // si se indico los tokens de tipo numerico generico:
        if (isdigit(lexer->chartter) && token_number != NULL) return lexer_parser_number(lexer);

        // si se indico los tokens de tipo identificador generico:
        else if (isalnum(lexer->chartter) != 0 && token_id != NULL) return lexer_advance_with(lexer, lexer_parser_id(lexer));

        else return token_analysis(lexer);
    }
    self = init_token_build(NULL);
    self->token = get(lexer->hash_table, build_token_special(TOKEN_EOF));
    if (self->token == NULL) {
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "#{FG:red}Error: #{FG:white}No se ha podido obtener el token EOF\n");
        return NULL;
    }
    return self;
}

Lexer_t backup_lexer(Lexer_t* lexer) {
    /*
     * permite hacer una copia del lexer para poder restaurarlo en caso de necesitarlo
     */
    return (Lexer_t){
        .chartter       = lexer->chartter,
        .data           = lexer->data,
        .index          = lexer->index,
        .size           = lexer->size,
        .hash_table     = lexer->hash_table,
        .list_id_tokens = lexer->list_id_tokens,
    };
}

/* Liberar el lexer */
void free_lexer(Lexer_t *lexer) {
    if (lexer->list_id_tokens != NULL) {

        // liberar la lista de tokens
        freeArrayList_struct((ArrayList **)&(lexer->list_id_tokens->Array));
    }
    if (lexer->hash_table != NULL) {
        // no es necesario liberar cada valor de las entradas de la tabla hash,
        // ya que cada entrada se asociaba con el valor ya liberado de las listas enlazadas
        freeHashTable(lexer->hash_table);
    }
}

#endif