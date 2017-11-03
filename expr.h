#ifndef EXPR_H_
#define EXPR_H_

//构建表达式栈的数据结构
#include "symtab.h"
typedef enum {
	TYPE_NUM, TYPE_VAR
}TYPE;
typedef struct {
	int kind;	//表示进栈的类型
	int num;
	symItem var;
}Datum;

Datum var_push(symItem var);
Datum num_push(int num);
Datum datum_pop();
void code_binary(int op);		//二元运算处理
void code_assign(symItem var);	//赋值运算处理
void code_print();				//打印语句的处理
#endif
