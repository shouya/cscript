CC = gcc
LEX = flex
YACC = bison

C_SRC = $(wildcard *.c)

LEX_SRC = $(wildcard *.l)
YACC_SRC = $(wildcard *.y)

LEX_OUT = $(LEX_SRC:.l=.c)
YACC_OUT_H = $(YACC_SRC:.y=.tab.h)
YACC_OUT_C = $(YACC_OUT_H:.tab.h=.tab.c)

CFLAGS = -Wall -std=c90 -pipe
LIBS = -lfl -lm
YACC_FLAGS = -d
LEX_FLAGS =

BUILD_TARGET = cscript

DEBUG=yes

ifeq ($(DEBUG), yes)
	CFLAGS += -g
	YACC_FLAGS += --debug --verbose
else	
	CFLAGS += -O3
endif

all: $(BUILD_TARGET)

$(BUILD_TARGET): $(C_SRC) $(LEX_OUT) $(YACC_OUT_C)
	$(CC) $(CFLAGS) -o$@ $^ $(LIBS)

$(LEX_OUT): $(LEX_SRC) $(YACC_OUT_H)
	$(LEX) $(LEX_FLAGS) -o $@ $<

$(YACC_OUT_H):
$(YACC_OUT_C): $(YACC_SRC)
	$(YACC) $(YACC_FLAGS) $<

.PHONY : clean
clean:
	/bin/rm -f $(YACC_OUT_H) $(YACC_OUT_C) $(LEX_OUT) $(BUILD_TARGET) *.o \
		*.output

rebuild: clean all


# This rugulation only use for doing unit testing 
.PHONY : test
test:
	gcc -Wall cscript_test.c cscript_htbl.c -o test