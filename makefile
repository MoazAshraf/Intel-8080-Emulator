CC = gcc
SRC = src
DIST = dist
MKDIR = mkdir
RM = rm

all: $(DIST)/asm

$(DIST)/asm: $(SRC)/asm.c $(SRC)/errors.o $(SRC)/loadfiles.o
	$(CC) -o $(DIST)/asm $(SRC)/asm.c $(SRC)/errors.o $(SRC)/loadfiles.o

$(SRC)/errors.o: $(SRC)/errors.c
	$(CC) -o $(SRC)/errors.o -c $(SRC)/errors.c

$(SRC)/loadfiles.o: $(SRC)/loadfiles.c
	$(CC) -o $(SRC)/loadfiles.o -c $(SRC)/loadfiles.c

clean:
	$(RM) -rf $(DIST)/*
	$(RM) -rf $(SRC)/*.o