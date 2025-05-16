#ifndef LEXPAR_GLOBAL_H
#define LEXPAR_GLOBAL_H

#ifndef UNUSED
#   if defined(__GNUC__) || defined(__clang__)
#      define UNUSED __attribute__((unused))
#   elif defined(_MSC_VER)
#      define UNUSED __pragma(warning(suppress:4101))
#   else
#      define UNUSED
#   endif
#endif

// para parametros que no se usaran bajo ciertas circustancias
#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void)(x)
#endif

#ifndef Object
typedef void* Object;
#define Object Object
#endif

#include "ast_c.h"
#include "lexer.h"
#include "token.h"

#endif