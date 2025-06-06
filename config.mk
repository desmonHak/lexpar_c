CC 			  = gcc
ARR			  = ar

VESRION_C     =11

PATH_SRC 	      = src
PATH_INCLUDE      = include
PATH_EXAMPLES	  = example

PATH_LIB		  	= lib
PATH_STRUCTS_DATA_C = $(PATH_LIB)/structs_data_C
PATH_DEBUG 		  	= $(PATH_STRUCTS_DATA_C)/DebugLibC
PATH_COLORS		  	= $(PATH_DEBUG)/colors-C-C-plus-plus

LINKER_FLAGS  	    = 									\
	-L. 			 -llexpar_c							\
	-L$(PATH_STRUCTS_DATA_C) -lstructs_data_c 			\
	-L$(PATH_DEBUG)  -ldebug 							\
	-L$(PATH_COLORS) -lcolors							\

INCLUDE_FLAGS = 										\
	-I. 												\
	-I$(PATH_INCLUDE)									\
	-I$(PATH_STRUCTS_DATA_C) 							\
	-I$(PATH_DEBUG) -I$(PATH_COLORS)

GLOBAL_CFLAGS = -std=c$(VESRION_C) $(INCLUDE_FLAGS) -masm=intel \
				-D_ExceptionHandler -fdiagnostics-color=always -D_GNU_SOURCE $(DEBUG_LINUX)

ifeq ($(OS_NAME),windows)
else
	GLOBAL_CFLAGS += -fPIC -D_GNU_SOURCE  -D_POSIX_C_SOURCE=200809L
endif

CFLAGS 		  =  $(GLOBAL_CFLAGS) -O3 -Wno-unused-parameter \
				-Wno-implicit-fallthrough -Wno-type-limits  \
				-Wno-unused-variable -Wno-pointer-sign

CFLAGS_DEBUG  =  $(GLOBAL_CFLAGS) -ggdb -fno-asynchronous-unwind-tables  	    	\
				-Wall -Wextra -pipe -O0 -D DEBUG_ENABLE      	          			\
				-fstack-protector-strong -Wpedantic -fno-omit-frame-pointer       	\
				-fno-inline -fno-optimize-sibling-calls -fdiagnostics-show-option 	\
				-fPIC 
				

ARR_FLAGS     = -rc

CFLAGS_EXAMPLES 		= $(CFLAGS) $(LINKER_FLAGS)
CFLAGS_EXAMPLES_DEBUG 	= $(CFLAGS_DEBUG) $(LINKER_FLAGS)

OBJECTS 	  = ast_c.o lexer.o token.o json_c.o
OBJECTS_DEBUG = ast_c_debug.o lexer_debug.o token_debug.o json_c_debug.o