OBJS = parse.o symtab.o lex.o

parse:  $(OBJS)
	gcc -o parse $(OBJS)

parse.o:  lex.h symtab.h parse.c
	gcc -c parse.c

lex.o:  lex.h lex.c
	gcc -c lex.c

symtab.o:  symtab.h symtab.c
	gcc -c symtab.c

clean:
	rm -f *.o parse
