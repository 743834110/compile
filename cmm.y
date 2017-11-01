%{
	#include <stdio.h>
	#include "lex.h"
	#include "symtab.h"
	#include <stdlib.h>
	
	extern int LINENUM;
	extern int COLNUM;
	extern Token token;
	int offset = 0;
	
	symItem symbol_def (symKind kind);
	symItem symbol_use (symKind kind);
	void code_binary(char* ID);
	
%}

%union{
	int val;		//���� 
	symItem sym;	//���ű��� 
}
%token <val> NUM <sym> ID
%token INT IF ELSE WHILE READ PRINT RETURN
%left AND OR 
%left LT LE GT GE EQ NE
%left '+' '-'
%left '*' '/'
%right NEG 		//ȡ��

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
		|		error		{yyclearin;}		//yyclearinΪ�궨�����
		;

var_def:		INT ids ';'
		;

ids:			ID			{$1 = symbol_def(K_VAR);}
		| 		ids ',' ID	{$3 = symbol_def(K_VAR);}
		;

stmt_if:		IF cond stmt else
		;
else:			
		|		ELSE stmt
		;
		
cond:			'(' expr ')'
		;

stmt_print:		PRINT exprs ';'
		;

exprs:			expr
		|		exprs ',' expr
		;
		
stmt_while:		WHILE cond stmt
		;

stmt_assign:	 ID {$1 = symbol_use(K_VAR);} '=' expr ';'	
		;



expr:			NUM					{$1 = token->integer;}
		|		ID					{$1 = symbol_use(K_VAR);}
		| 		'-' expr %prec NEG	{code_binary("-");}
		|		expr '+' expr		{code_binary("+");}
		|		expr '-' expr		{code_binary("-");}
		|		expr '*' expr		{code_binary("*");}
		|		expr '/' expr		{code_binary("/");}
		|		expr LT  expr		{code_binary("<");}
		|		expr LE	 expr		{code_binary("<=");}
		|		expr GT	 expr		{code_binary(">");}
		|		expr GE	 expr		{code_binary(">=");}
		|		expr AND expr		{code_binary("&&");}
		|		expr OR	 expr		{code_binary("||");}
		|		'(' expr ')'		{}
		;

%%

#include <stdio.h>

void code_binary(char* ID){
	
}

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
		
		


 


