#include <stdlib.h> 
#include "symtab.h"
#include <string.h>
#include <stdio.h>

static symItem heading = NULL;

symItem sym_insert (char *ID, symKind kind, int offset){
	symItem s = sym_find(ID);
	if (s != NULL)
		return s;
	s = (symItem)malloc(sizeof(*s));
	s->ID = ID;
	s->offset = offset;
	s->kind = kind;
	s->next = heading;
	heading = s;
	return s;
}

symItem sym_find (char *ID){
	symItem s ;
	for (s = heading; s != NULL; s = s->next)
		if (strcmp(ID, s->ID) == 0)
			break;
	return s;
}

void print_symTab(){
	symItem s ;
	for (s = heading; s != NULL; s = s->next)
		printf("%s  ", s->ID);
	printf("\n");	
}
