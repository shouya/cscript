CC = gcc
LEX = flex
YACC = bison

C_SRC =  # $(wildcard *.c)

LEX_SRC = $(wildcard *.l)
YACC_SRC = $(wildcard *.y)

LEX_OUT = $(LEX_SRC:.l=.c)
YACC_OUT_H = $(YACC_SRC:.y=.tab.h)
YACC_OUT_C = $(YACC_OUT_H:.tab.h=.tab.c)

CFLAGS = -Wall -std=c90 -pipe
LIBS = -lfl -lm

BUILD_TARGET = cscript

all: $(BUILD_TARGET)

$(BUILD_TARGET): $(C_SRC) $(LEX_OUT) $(YACC_OUT_C)
	$(CC) $(CFLAGS) -o$@ $^ $(LIBS)

$(LEX_OUT): $(LEX_SRC) $(YACC_OUT_H)
	$(LEX) -o $@ $<

$(YACC_OUT_H):
$(YACC_OUT_C): $(YACC_SRC)
	$(YACC) -d $<

.PHONY : clean
clean:
	/bin/rm -f $(YACC_OUT_H) $(YACC_OUT_C) $(LEX_OUT) $(BUILD_TARGET) *.o

rebuild: clean all

