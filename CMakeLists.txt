cmake_minimum_required(VERSION 3.28)
project(lexpar_c C)

set(CMAKE_C_STANDARD 11)

# Permitir establecer modo Debug o Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Configuraciones específicas por tipo de compilación
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG_ENABLE -O0 -Wall -Wpedantic -D_ExceptionHandler")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG  -D_ExceptionHandler")

include_directories(include)
include_directories(lib/structs_data_C)
include_directories(lib/structs_data_C/CIterators/include)
include_directories(lib/structs_data_C/DebugLibC)
include_directories(lib/structs_data_C/DebugLibC/colors-C-C-plus-plus)

# examples/code.c
add_executable(code
        examples/code.c
        include/ast_c.h
        include/global.h
        include/lexer.h
        include/token.h
        lib/structs_data_C/CIterators/include/CIterators.h
        lib/structs_data_C/CIterators/include/CSortting.h
        lib/structs_data_C/CIterators/src/CIterators.c
        lib/structs_data_C/CIterators/src/CSortting.c
        lib/structs_data_C/DebugLibC/colors-C-C-plus-plus/src/colors.c
        lib/structs_data_C/DebugLibC/colors-C-C-plus-plus/colors.h
        lib/structs_data_C/DebugLibC/src/debug_c.c
        lib/structs_data_C/DebugLibC/src/error_c.c
        lib/structs_data_C/DebugLibC/src/time_code.c
        lib/structs_data_C/DebugLibC/debug_c.h
        lib/structs_data_C/DebugLibC/error_c.h
        lib/structs_data_C/DebugLibC/time_code.h
        lib/structs_data_C/src/array-list.c
        lib/structs_data_C/src/ast.c
        lib/structs_data_C/src/hash-table.c
        lib/structs_data_C/src/matriz-list.c
        lib/structs_data_C/src/vector-list.c
        lib/structs_data_C/array-list.h
        lib/structs_data_C/ast.h
        lib/structs_data_C/hash-table.h
        lib/structs_data_C/matriz-list.h
        lib/structs_data_C/vector-list.h
        src/ast_c.c
        src/lexer.c
        src/token.c)

# examples/json.c
add_executable(json
        examples/json.c
        include/ast_c.h
        include/global.h
        include/lexer.h
        include/token.h
        lib/structs_data_C/CIterators/include/CIterators.h
        lib/structs_data_C/CIterators/include/CSortting.h
        lib/structs_data_C/CIterators/src/CIterators.c
        lib/structs_data_C/CIterators/src/CSortting.c
        lib/structs_data_C/DebugLibC/colors-C-C-plus-plus/src/colors.c
        lib/structs_data_C/DebugLibC/colors-C-C-plus-plus/colors.h
        lib/structs_data_C/DebugLibC/src/debug_c.c
        lib/structs_data_C/DebugLibC/src/error_c.c
        lib/structs_data_C/DebugLibC/src/time_code.c
        lib/structs_data_C/DebugLibC/debug_c.h
        lib/structs_data_C/DebugLibC/error_c.h
        lib/structs_data_C/DebugLibC/time_code.h
        lib/structs_data_C/src/array-list.c
        lib/structs_data_C/src/ast.c
        lib/structs_data_C/src/hash-table.c
        lib/structs_data_C/src/matriz-list.c
        lib/structs_data_C/src/vector-list.c
        lib/structs_data_C/array-list.h
        lib/structs_data_C/ast.h
        lib/structs_data_C/hash-table.h
        lib/structs_data_C/matriz-list.h
        lib/structs_data_C/vector-list.h
        src/ast_c.c
        src/lexer.c
        src/token.c)