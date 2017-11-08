#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "expr.h"
#include "cmm.tab.h"

Datum datum[32];
int top = 0;
//进行其它地方定义的变量
extern int offset;

//变量进栈
Datum var_push(symItem var){
	Datum d;
	d.kind = TYPE_VAR;
	d.var = var;
	datum[top++] = d;
}
//整型进栈
Datum num_push(int num){
	Datum d;
	d.kind = TYPE_NUM;
	d.num = num;
	datum[top++] = d;
}
//Datum栈:尚未进行错误的处理
Datum datum_pop(){
	if (top > 0){
		return datum[--top];
	}
	else{
		printf("var stack is empty!!!\n");
		exit(1);
	}
}
//generated temporary var
static Datum new_var(){
	static int index = 0;
	static char ch = 't';
	/*临时变量长度最大为256位*/
	char str[256];		
	sprintf(str,"%c%d", ch, index++);
	symItem sym = sym_insert(strdup(str), K_VAR, offset);
	offset += 4;
	return var_push(sym);
}
char* toString(Datum d){
	char s[256];
	if (d.kind == TYPE_NUM)
		sprintf(s, "%d", d.num);
	else
		sprintf(s, "%s", d.var->ID);
	return strdup(s);
}

void code_cmp(char* s, char* s1, char* s2, char* s3){
	printf("\t%s = %s %s %s\n", s, s1, s2, s3);
}
//二元运算处理
void code_binary(int op){
	Datum d1 = datum_pop(), d2 = datum_pop(), d = new_var();
	char* s1 = toString(d1);
	char* s2 = toString(d2);
	switch(op){
		case '+':
			printf("\t%s = %s + %s\n", d.var->ID, s2, s1);
			break;
		case '-':
			printf("\t%s = %s - %s\n", d.var->ID, s2, s1);
			break;
		case '*':
			printf("\t%s = %s * %s\n", d.var->ID, s2, s1);
			break;
		case '/':
			printf("\t%s = %s / %s\n", d.var->ID, s2, s1);
			break;
		case LT:
			code_cmp(d.var->ID, s2, "<", s1);
			break;
		case LE:
			code_cmp(d.var->ID, s2, "<=", s1);
			break;
		case GT:
			code_cmp(d.var->ID, s2, ">", s1);
			break;
		case GE:
			code_cmp(d.var->ID, s2, ">=", s1);
			break;
		case EQ:
			code_cmp(d.var->ID, s2, "==", s1);
			break;
		case NE:
			code_cmp(d.var->ID, s2, "!=", s1);
			break;

		default:{
				printf("expected resolve symbol: %s\n", op);
				exit(1);
			}	
	}
	
	
	
}
//赋值运算处理:结果出栈,
void code_assign(symItem var){
	Datum d = datum_pop();
	char* s = toString(d);
	printf("\t%s = %s\n", var->ID, s);

}	

//打印语句处理
void code_print(){
	char* s = toString(datum_pop());
	printf("\tprint %s \n", s);
}
