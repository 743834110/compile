#include <stdio.h>
#include "lex.h"

void expr();
void stmt();
void stmt_block();

static Token token = NULL;
extern int LINENUM;
// ��ʱû�п����ַ�������
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
	// ������ӵ������ܻ�Ӱ�칦��	
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
	 							 
	// if �����ź��ж������: '{' , ';'
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
	// else ����ڶ������ �� ��{��������������if��,'stmt' 
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
				stmt_if();			 // else ������Ը��� if  
				break;
			default:
				stmt();
		}
	}
}

// ����� 
void stmt_block(){
	token = getToken();  // skip {
	while(token->sym != RC && token->sym != END && token->sym != ELSE)  // ����else�����жϣ�����û�������ŵ�if���ļ�� 
		stmt();  
	if (token->sym != RC)
		errExit("expect  '}'"); 
	token = getToken();  // skip }
}
 
// while ѭ����� 
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
		errExit("expect '{'");	// ��ֹû�з��������žͽ��������ж���� 
	stmt_block(); 	  
} 

// �������� 
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

//��ʱ�����Ǻ����Ķ��� ����Ϊʶ���Ѿ��ں����ڲ����� 
void program(){
	token = getToken();
	// һ�����߶�����
	while(token->sym != END)
		stmt();	
	
}
char eee[]="-1{}";
void print_element() {
	
	token = getToken();   //token ���ڿɷ���NULL
	do {
		printf("��%d�У�%s\t%s\n",LINENUM,token->ID,sym_names[token -> sym]);
	} while((token = getToken())->sym != END) ;
}

int main(int argc, char* argv[]) {
//	file = argc == 1?stdin:fopen(argv[1],"r+");
	file = fopen("..\\dirlist.txt","r+");
//	print_element();
	program();
	return 0;
}



