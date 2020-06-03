# programs
CC = gcc
MKDIR = mkdir
RM = rm -f

# directories and names
srcdir = src
distdir = dist
dist = $(distdir)/asm
testsdir = tests
testsdist = $(testsdir)/check_tests


# list of object files to link in executable
OBJS =
OBJS += $(srcdir)/asm.o
OBJS += $(srcdir)/assemble.o
OBJS += $(srcdir)/io_utils.o
OBJS += $(srcdir)/mnemonics.o
OBJS += $(srcdir)/tokenize.o

# builds the executable
$(dist): $(OBJS)
	$(CC) -o $(dist) $(OBJS)

all: $(dist)

# compiles source files into object files
$(OBJS): %.o : %.c
	$(CC) -c $< -o $@


# list of test object files to link in test runner executable
TEST_OBJS =
TEST_OBJS += $(testsdir)/check_tests.o
TEST_OBJS += $(testsdir)/test_get_token.o
TEST_OBJS += $(srcdir)/tokenize.o
TEST_OBJS += $(srcdir)/mnemonics.o
TEST_OBJS += $(testsdir)/test_read_until_eof.o
TEST_OBJS += $(srcdir)/io_utils.o

# compiles and runs unit tests
check: $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -lcheck -lm -lpthread -lrt -lsubunit -o $(testsdist)
	$(testsdist)

# compiles test source files into object files
$(TEST_OBJS): %.o : %.c
	$(CC) -c $< -o $@


# removes any output files (object files, executable)
clean:
	$(RM) $(srcdir)/*.o
	$(RM) $(dist)
	$(RM) $(testsdir)/*.o
	$(RM) $(testsdist)