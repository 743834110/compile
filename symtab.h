#ifndef SYMTAB_H_
#define SYMTAB_H_

typedef enum {
	K_VAR, K_FUN, K_PAR, KEYWORD, K_OP
}symKind;
typedef struct _symTab {
	char *ID;
	symKind kind;
	int offset;
	struct _symTab *next;
} * symItem;

symItem sym_insert (char *ID, symKind kind, int offset);
symItem sym_find (char *ID);
void print_symTab();
#endif 
