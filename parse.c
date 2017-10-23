#include <stdio.h>
#include "lex.h"

void expr();
void stmt();
void stmt_block();

static Token token = NULL;
extern int LINENUM;
// 暂时没有考虑字符串常量
char * sym_names[] = {
	"NON", "LP", "RP", "LC", "LR", "COMMA", "SEMI", "ASSIGN", "ID",
	"NUM", "INT", "IF", "ELSE", "WHILE", "PRINT", "RETURN", "READ",
	"NOT", "ADD", "SUB", "MUL", "DIV", "LT", "LE", "GT", "GE", "EQ", "NE",
	"END"
};
FILE *file = NULL;
int level = 0;

void factor(){
	switch(token->sym){
		case ID:{
			token = getToken();			// skip ID
			break;
		}
		case NUM:{
			token = getToken();			// skip NUM 
			break;
		}
		case LP:{
			token = getToken();			// skip LP
			expr();
			if (token->sym != RP)
				errExit("expect ')'");
			token = getToken();			// skip RP
			break;
		}
		default:{
			printf("%s",token->ID);
			errExit("bad symbol");
			break;
		}
	}
}


// "LT", "LE", "GT", "GE", "EQ", "NE" 
void term(){
	factor();
	while(token -> sym == MUL || token -> sym == DIV || token->sym == LT || token->sym == LE || token->sym == GT || token->sym == GE || token->sym == EQ || token->sym == NE){
		token = getToken();					// skip * or /
		factor();	
	}
}

void expr(){
	term();
	while (token->sym == ADD || token ->sym == SUB){
		token = getToken();					// skip + or -  
		term();	
	}	
	// 以下添加的语句可能会影响功能	
}

void stmt_assign(){
	token = getToken();  			//skip ID
	if (token->sym != ASSIGN)
		errExit("expect '='");
	token = getToken();				//skip ASSIGN
	
	expr();					
	if (token->sym != SEMI)
		errExit("expect ';'");
	token = getToken();				// skip SEMI 	
}

void stmt_print(){
	token = getToken();				// skip PRINT 
	expr();
	while(token->sym == COMMA){
		token = getToken();			// skip COMMA
		expr();
	}
	if (token->sym != SEMI)
		errExit("expect ';'"); 
	token = getToken();				// skip SEMI
	
}

void stmt_if(){
	token = getToken();				// skip IF
	if (token->sym != LP)
		errExit("expect '('");
	token = getToken();				// skip LR
	expr();
	if (token->sym != RP)
		errExit("expect ')'");
	token = getToken();				// skip RP 
	 							 
	// if 右括号后有多种情况: '{' , ';'
	switch(token->sym){
		case LC:  					
			stmt_block();
			break; 
		case SEMI:  				
			token = getToken(); 	// skip SEMI
			break;
		default:{
			stmt();
			break;
		}
	} 
	// else 后存在多种情况 ； ‘{’，‘；’，‘if’,'stmt' 
	if (token->sym == ELSE){
		token = getToken();			// skip ELSE
		switch (token->sym){
			case LC:
				stmt_block();
				break;
			case SEMI:
				token = getToken();  // skip SEMI
				break;
			case IF:
				stmt_if();			 // else 后面可以跟着 if  
				break;
			default:
				stmt();
		}
	}
}

// 程序块 
void stmt_block(){
	token = getToken();  // skip {
	while(token->sym != RC && token->sym != END && token->sym != ELSE)  // 新增else条件判断，用于没有收括号的if语句的检查 
		stmt();  
	if (token->sym != RC)
		errExit("expect  '}'"); 
	token = getToken();  // skip }
}
 
// while 循环语句 
void stmt_while(){
	token = getToken();			// skip while
	if (token->sym != LP)
		errExit("expect '('");
	token = getToken();			// skip (
	expr();
	if (token->sym != RP)
		errExit("expect ')'"); 
	token = getToken();			// skip )
	if (token->sym == SEMI){
		token = getToken();		// skip ; 
		return;					// if ';' return else stmt_block
	}
	if (token->sym != LC)
		errExit("expect '{'");	// 防止没有发现左花括号就进入程序块判断语句 
	stmt_block(); 	  
} 

// 变量定义 
void var_def(){
	token = getToken();			// skip int
	loop:{
		if (token->sym != ID)
			errExit("expect ID");
		token = getToken();			// skip ID
		if (token->sym != ASSIGN)
			errExit("expect '='");
		token = getToken();			// skip '=' 
		expr();
		while (token->sym == COMMA){
			token = getToken();		// skip COMMA
			goto loop; 
		}
	}
	if (token->sym != SEMI)
		errExit("expect ';'");
	token = getToken();				// skip SEMI
} 

void stmt(){
	switch(token->sym){
		case ID:{
			stmt_assign();
			break;
		} 
		case IF:{
			stmt_if();
			break;
		}
		case PRINT:{
			stmt_print();
			break;
		}
		case LC:{
			stmt_block();
			break;
		}
		case WHILE:
			stmt_while();
			break;
		case INT:
			var_def(); 
			break; 
		default:
			errExit("bad symbol");
			break;
		
	}
} 

//暂时不考虑函数的定义 ：认为识别已经在函数内部进行 
void program(){
	token = getToken();
	// 一个或者多个语句
	while(token->sym != END)
		stmt();	
	
}
char eee[]="-1{}";
void print_element() {
	
	token = getToken();   //token 现在可返回NULL
	do {
		printf("第%d行：%s\t%s\n",LINENUM,token->ID,sym_names[token -> sym]);
	} while((token = getToken())->sym != END) ;
}

int main(int argc, char* argv[]) {
//	file = argc == 1?stdin:fopen(argv[1],"r+");
	file = fopen("..\\dirlist.txt","r+");
//	print_element();
	program();
	return 0;
}



