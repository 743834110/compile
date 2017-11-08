%{
	#include <stdio.h>
	#include "lex.h"
	#include "symtab.h"
	#include <stdlib.h>
	#include "expr.h"
	#include "ctrl.h"
	
	extern int LINENUM;
	extern int COLNUM;
	extern Token token;
	int offset = 0;
	
	symItem symbol_def (symKind kind);
	symItem symbol_use (symKind kind);
	
%}

%union{
	int val;		//整数 
	symItem sym;	//符号表项 
}
%token <val> NUM <sym> ID
%token INT IF ELSE WHILE READ PRINT RETURN
%left AND OR 
%left LT LE GT GE EQ NE
%left '+' '-'
%left '*' '/'
%right NEG 		//取负

%%
program:		stmts
		;

stmt_blocks:	'{'	stmts '}'

		;
stmts:			
		| 		stmts stmt
		;
stmt:			var_def
		|		stmt_if
		|		stmt_print
		|		stmt_while 
		|		stmt_assign
		|		stmt_blocks
		|		error		{yyclearin;}		//yyclearin为宏定义错误
		;

var_def:		INT ids ';'
		;

ids:			ID			{$1 = symbol_def(K_VAR);var_push($1);}
		| 		ids ',' ID	{$3 = symbol_def(K_VAR);var_push($3);}
		;

stmt_if:		IF cond stmt else	{code_if_end();}
		;
else:			
		|		ELSE	{code_else_part();}	stmt		
		;
		
cond:			'(' expr ')'	{code_cond();}
		;

stmt_print:		PRINT exprs ';'								{code_print();}
		;

exprs:			expr
		|		exprs ',' expr
		;
		
stmt_while:		WHILE	{code_while_begin();} cond stmt	{code_while_end();}
		;

stmt_assign:	 ID {$1 = symbol_use(K_VAR);} '=' expr ';'	{code_assign($1);}
		;



expr:			NUM					{$1 = token->integer;num_push($1);}
		|		ID					{$1 = symbol_use(K_VAR);var_push($1);}
		| 		'-' expr %prec NEG	{}
		|		expr '+' expr		{code_binary('+');}
		|		expr '-' expr		{code_binary('-');}
		|		expr '*' expr		{code_binary('*');}
		|		expr '/' expr		{code_binary('/');}
		|		expr LT  expr		{code_binary(LT);}
		|		expr LE	 expr		{code_binary(LE);}
		|		expr GT	 expr		{code_binary(GT);}
		|		expr GE	 expr		{code_binary(GE);}
		|		expr AND expr		{}
		|		expr OR	 expr		{}
		|		'(' expr ')'		{}
		;

%%

#include <stdio.h>

symItem symbol_def (symKind kind){
	symItem s = sym_find(token->ID);

	if (s == NULL){
		s = sym_insert(token->ID, kind, offset);
		offset += 4;
	}
	else
		yyerror("redefined");
	return s;
}
symItem symbol_use (symKind kind){
	symItem s = sym_find(token->ID);
	if (s == NULL)
		yyerror("undefined");
	return s;
}

int yyerror(const char* msg){
	
	fprintf(stderr,"%d:%d near '%s': %s\n", LINENUM, COLNUM, token->ID, msg);
	exit(1);
	return 0;
}

int main(){
	return yyparse();
}
		
		


 


