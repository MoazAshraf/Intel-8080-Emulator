# programs
CC = gcc
MKDIR = mkdir
RM = rm

# directories and names
srcdir = src
distdir = dist
testsdir = tests
distname = $(distdir)/asm

# list of object files to link in executable
OBJS =
OBJS += $(srcdir)/asm.o
OBJS += $(srcdir)/assemble.o
OBJS += $(srcdir)/errors.o
OBJS += $(srcdir)/loadfiles.o
OBJS += $(srcdir)/mnemonics.o
OBJS += $(srcdir)/tokenize.o

# builds the executable
$(distname): $(OBJS)
	$(CC) -o $(distname) $(OBJS)

# compiles source files into object files
$(OBJS): %.o : %.c
	$(CC) -c $< -o $@

# removes any output files (object files, executable)
clean:
	$(RM) $(distname)
	$(RM) $(srcdir)/*.o