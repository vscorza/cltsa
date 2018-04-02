CC=c99
CFLAGS= -Wall -Wextra -pedantic -O0 -g -lm -Wno-unused-variable -Wno-unused-parameter
NASM=nasm
NASMFLAGS=-f elf64 -g -F DWARF

all: main tester

main: main.c automaton_c.o automaton_asm.o
	$(CC) $(CFLAGS) $^ -o $@

tester: tester.c automaton_c.o automaton_asm.o
	$(CC) $(CFLAGS) $^ -o $@

automaton_asm.o: automaton.asm
	$(NASM) $(NASMFLAGS) $< -o $@

automaton_c.o: automaton.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f main tester
	rm -f salida.caso.*

