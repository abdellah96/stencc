include config.mk

.POSIX:
.SUFFIXES: .y .c .o

BIN = stencc
OBJ = grammar.o lex.o sym.o ast.o quad.o asm_mips.o main.o

all: options $(BIN)

options:
	@echo $(BIN) build options:
	@echo "CC       = $(CC)"
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"

$(BIN): $(OBJ)
	@echo LD $@
	@$(LD) -o $@ $^ $(LDFLAGS)

.c.o:
	@echo CC $<
	@$(CC) -o $@ $< $(CFLAGS)

.y.c:
	@echo YACC $<
	@$(YACC) -o $@ $<

test:
	tests/check_all.sh
	#tests/valgrind.sh
	tests/main_test.sh

clean:
	@echo cleaning
	rm -f $(BIN) *.o grammar.output grammar.h mips.out

.PHONY: all options test clean
