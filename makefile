CC = gcc
SRC = src
DIST = dist
TESTS = tests
TESTS_OUT = tests/out
MKDIR = mkdir
RM = rm

all: clean $(DIST)/asm tests


$(DIST)/asm: $(SRC)/asm.c $(SRC)/errors.o $(SRC)/loadfiles.o \
            $(SRC)/tokenize.o $(SRC)/mnemonics.o
	$(CC) -o $(DIST)/asm $(SRC)/asm.c $(SRC)/errors.o $(SRC)/loadfiles.o \
            $(SRC)/tokenize.o $(SRC)/mnemonics.o

$(SRC)/errors.o: $(SRC)/errors.c
	$(CC) -o $(SRC)/errors.o -c $(SRC)/errors.c

$(SRC)/loadfiles.o: $(SRC)/loadfiles.c
	$(CC) -o $(SRC)/loadfiles.o -c $(SRC)/loadfiles.c

$(SRC)/tokenize.o: $(SRC)/tokenize.c
	$(CC) -o $(SRC)/tokenize.o -c $(SRC)/tokenize.c

$(SRC)/mnemonics.o: $(SRC)/mnemonics.c
	$(CC) -o $(SRC)/mnemonics.o -c $(SRC)/mnemonics.c


tests: $(TESTS_OUT)/test_stm $(TESTS_OUT)/test_arg $(TESTS_OUT)/test_tok

$(TESTS_OUT)/test_stm: $(TESTS)/test_stm.c $(SRC)/errors.o $(SRC)/tokenize.o \
					   $(SRC)/mnemonics.o
	$(CC) -o $(TESTS_OUT)/test_stm $(SRC)/errors.o $(SRC)/tokenize.o \
	        $(SRC)/mnemonics.o $(TESTS)/test_stm.c

$(TESTS_OUT)/test_arg: $(TESTS)/test_arg.c $(SRC)/errors.o $(SRC)/tokenize.o \
					   $(SRC)/mnemonics.o
	$(CC) -o $(TESTS_OUT)/test_arg $(SRC)/errors.o $(SRC)/tokenize.o \
	        $(SRC)/mnemonics.o $(TESTS)/test_arg.c

$(TESTS_OUT)/test_tok: $(TESTS)/test_tok.c $(SRC)/errors.o $(SRC)/tokenize.o \
					   $(SRC)/mnemonics.o
	$(CC) -o $(TESTS_OUT)/test_tok $(SRC)/errors.o $(SRC)/tokenize.o \
	        $(SRC)/mnemonics.o $(TESTS)/test_tok.c

clean:
	$(RM) -rf $(DIST)/*
	$(RM) -rf $(SRC)/*.o
	$(RM) -rf $(TESTS_OUT)/*