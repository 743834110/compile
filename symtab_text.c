#include "symtab.h"
#include <stdio.h>

char * words[12] = {
   "address", "audio", "binary", "compile", "datum", "grammar",
   "graph", "memory", "path", "tree", "unary", "video"
};

int main(){
	int i;
	printf("[insert]\n"); //·´¶Ô
	for (i = 0; i < 12; i ++){
		printf("%s  ", words[i]);
		sym_insert(words[i], K_VAR,0);
	}
	printf("\n\n[find]\n");
	
	char * a[4] = { "sort", "unary", "binary", "tirnary" };
	for (i = 0; i < 4; i ++){
		if (sym_find(a[i]) != NULL)
			printf("%-8s:found.\n", a[i]);
		else
			printf("%-8s:not found.\n",a[i]);
	}
	return 0;
}
