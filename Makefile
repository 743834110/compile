OBJS = cmm.tab.o symtab.o lex.o

cmm:  $(OBJS)
	gcc -o cmm $(OBJS)

cmm.tab.h cmm.tab.c: cmm.y
	bison -d cmm.y

cmm.tab.o:  lex.h symtab.h cmm.tab.h cmm.tab.c
	gcc -c cmm.tab.c

lex.o: lex.h  lex.c
	gcc -c lex.c

symtab.o:  symtab.h symtab.c
	gcc -c symtab.c

clean:
	rm -f *.o parse *.stackdump cmm
