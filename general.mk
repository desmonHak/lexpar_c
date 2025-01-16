include config.mk

generate_lib: libstructs_data_c.a $(TARGET).a
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

$(TARGET).a: $(OBJECTS)
	echo "generando librerias estatica... $@"
	$(ARR) $(ARR_FLAGS) $@ $^
	ranlib $@

ast.o: $(PATH_SRC)/ast.c
	$(CC) $(CFLAGS) -c $^ -o $@

lexer.o: $(PATH_SRC)/lexer.c
	$(CC) $(CFLAGS) -c $^ -o $@

token.o: $(PATH_SRC)/token.c
	$(CC) $(CFLAGS) -c $^ -o $@

cleanobj:
	$(RM) $(RMFLAGS) $(OBJECTS)
	$(MAKE) -C ./$(PATH_STRUCTS_DATA_C) -f $(MAKE_NAME) cleanobj

cleanall: cleanobj
	$(RM) $(RMFLAGS) $(TARGET).o $(TARGET).a \
	$(TARGET_DEBUG).a
	$(MAKE) -C ./$(PATH_STRUCTS_DATA_C) -f $(MAKE_NAME) cleanall

.SILENT: clean cleanobj cleanall
.IGNORE: cleanobj cleanall
.PHONY:  cleanobj cleanall