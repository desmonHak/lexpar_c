include config.mk

generate_lib: libstructs_data_c.a $(TARGET).a
	ar -t $(TARGET).a

generate_lib_debug: libstructs_data_c_debug.a $(TARGET)_debug.a
	ar -t $(TARGET).a

all: generate_lib
	$(MAKE) -C . -f $(MAKE_NAME) examples

examples: generate_lib
	$(CC) examples/code.c $(CFLAGS_EXAMPLES) -o code.$(EXTENSION)
	$(CC) examples/json.c $(CFLAGS_EXAMPLES) -o json.$(EXTENSION)
examples_debug: generate_lib
	$(CC) examples/code.c $(CFLAGS_EXAMPLES_DEBUG) -o code.$(EXTENSION)
	$(CC) examples/json.c $(CFLAGS_EXAMPLES_DEBUG) -o json.$(EXTENSION)

libstructs_data_c.a:
	echo "generando librerias estatica... $@"
	$(MAKE) -C ./$(PATH_STRUCTS_DATA_C) -f $(MAKE_NAME)

libstructs_data_c_debug.a:
	echo "generando librerias estatica... $@"
	$(MAKE) -C ./$(PATH_STRUCTS_DATA_C) -f $(MAKE_NAME) generate_lib_debug

$(TARGET).a: $(OBJECTS)
	echo "generando librerias estatica... $@"
	$(ARR) $(ARR_FLAGS) $@ $^
	ranlib $@

$(TARGET)_debug.a: $(OBJECTS_DEBUG)
	$(ARR) $(ARR_FLAGS) $(TARGET).a $^
	ranlib $(TARGET).a

ast.o: $(PATH_SRC)/ast.c
	$(CC) $(CFLAGS) -c $^ -o $@

lexer.o: $(PATH_SRC)/lexer.c
	$(CC) $(CFLAGS) -c $^ -o $@

token.o: $(PATH_SRC)/token.c
	$(CC) $(CFLAGS) -c $^ -o $@

ast_debug.o: $(PATH_SRC)/ast.c
	$(CC) $(CFLAGS_DEBUG) -c $^ -o $@

lexer_debug.o: $(PATH_SRC)/lexer.c
	$(CC) $(CFLAGS_DEBUG) -c $^ -o $@

token_debug.o: $(PATH_SRC)/token.c
	$(CC) $(CFLAGS_DEBUG) -c $^ -o $@

cleanobj:
	$(RM) $(RMFLAGS) $(OBJECTS) $(OBJECTS_DEBUG)
	$(MAKE) -C ./$(PATH_STRUCTS_DATA_C) -f $(MAKE_NAME) cleanobj

cleanall: cleanobj
	$(RM) $(RMFLAGS) *.o $(TARGET).a \
	$(TARGET_DEBUG).a
	$(MAKE) -C ./$(PATH_STRUCTS_DATA_C) -f $(MAKE_NAME) cleanall

.SILENT: clean cleanobj cleanall
.IGNORE: cleanobj cleanall
.PHONY:  cleanobj cleanall