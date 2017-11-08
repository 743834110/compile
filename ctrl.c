#include <stdio.h>
#include "expr.h"
#include "ctrl.h"

static int stack[128];		//标号栈
static int top = 0;		//栈指针

static int new_var(){
	static int index = 1;
	return index++;
}

static int pop(){
	return stack[--top];
}

static void push(int index){
	stack[top++] = index;
}

void code_cond(){
	Datum d = datum_pop();
	char* str = toString(d);
	char label[256];
	int index = new_var();
	push(index);
	sprintf(label,".L%d", index);
	printf("\tif (! %s) goto %s;\n", str, label);
}

void code_if_end(){
	char label[256];
	int index = pop();
	sprintf(label, ".L%d", index);
	printf("%s:\n", label);
}

void code_else_part(){
	int index_1 = new_var(), index_2 = pop();
	char label_1[256], label_2[256];
	push(index_1);
	sprintf(label_1, ".L%d", index_1);
	sprintf(label_2, ".L%d", index_2);
	printf("\tgoto %s;\n", label_1);
	printf("%s:\n", label_2);
}

void code_while_begin(){
	int index_1 = new_var();
	char label_1[256];
	sprintf(label_1, ".L%d", index_1);
	push(index_1);
	printf("%s:\n", label_1);
}

void code_while_end(){
	int index_1 = pop(), index_2 = pop();
	char label_1[256], label_2[256];
	sprintf(label_1, ".L%d", index_1);
	sprintf(label_2, ".L%d", index_2);
	printf("\tgoto %s;\n", label_2);
	printf("%s:\n", label_1);	
}
