#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "symtab.h"
#include "cmm.tab.h"
#include "lex.h"
#define BUFLEN 20

typedef struct{
	char *ID;
	int sym; 
}Keyword; 

// file ����������lex.hͷ�ļ��� 
int LINENUM = 1;                        // �к� 
int COLNUM = 0;                         // �к� 
char buff[BUFLEN];                      // ���ļ��ж�ȡ���ַ�����Ŀ
char *pos = buff + BUFLEN;          // ָ��ǰ
int size = 0;                               //��ǰ�����������ַ��� 
int lastChar;
Token token;						//��Ϊ�﷨�ļ�cmm.y׼��token 
FILE *file = NULL;

// ���ڼ���Ƿ�Ϊ�ؼ��ֵĹؼ��ֵ�symӳ��⣬���ٶ����ж� 
Keyword keywords[] = {
	{"int", INT},
	{"if", IF},
	{"else", ELSE},
	{"while", WHILE},
	{"print", PRINT},
	{"return", RETURN},
	{"read", READ},
	{"NULL", 0} 	
};

//���ڻ�ȡ��һ���ַ�������ͳ�Ƶ�ǰ�����ַ����ڵ��кź��к� 
char getChar(){
	if (pos == buff + BUFLEN ){  //��ʾ�������Ѿ������Ѿ�ʹ������ˣ���Ҫ���¶�ȡ�ļ����������� 
		pos = buff;
		size = fread(buff,1,BUFLEN,file); 	 
		if (size != BUFLEN)
			fclose(file);
	} 
	if (pos < buff + size  && pos < buff + BUFLEN){
		if (lastChar == '\n'){
			LINENUM ++;
			COLNUM = 0;
		}
		COLNUM ++;
		lastChar = *pos ;
		return *pos++;	
	}
	 
	return EOF;  // �ļ����ݶ��꣬������-1 
}
//���Ѷ��ַ��˻ص��������� 
void ungetChar(){
	pos--; // ָ����ˣ���ʾ���ַ�Э�� 
	lastChar = *(pos - 1);	   // ��һ���ַ�ҲҪ���Ż��� 
	//�����ǰָ����ָ����ַ�Ϊ��\n��Ϊ��֤�кŵ���ȷ�ԣ�LINENUMӦ��ȥ1
	COLNUM--;
}

int getNumber(){
	int var = 0;
	char ch;
	while((ch = getChar()) != EOF && isdigit(ch) )
		var  = var * 10 + ch - '0';
	ungetChar();
	return var;
}

int keyword_lookup(char *ID){
	Keyword *p = keywords;
	while(strcmp(p -> ID,ID) != 0 && strcmp(p -> ID,"NULL") != 0) // NON��NULL��ͳһ 
		p ++;
	return p -> sym; 	
}

Token newToken(char *ID,int sym){
	Token token = (Token)malloc(sizeof(*token));
	token->ID = strdup(ID);
	token->sym = sym;
	return token;
}

// ���ע�ͣ���ע�͵����հ��ַ������� 
int check_comment(int ch){
	if (ch != '/')
	 	return 0;  //0����Ϊ���ù���
	char ch2 = getChar(); 
	if (ch2 == '/'){  // ��֪Ϊ����ע�ͣ��������г��ֵ��κ��ַ�����������  
		 
		 while((ch2 = getChar()) != EOF && ch2 != '\n');
		  return 1;
	}
	else if (ch2 == '*'){  //����֪Ϊ����ע�� 
		for (ch = getChar(); ch != EOF; ch = getChar()){
			if (ch == '*'){  //��鵽��*������ע�ͣ����������״̬ 
				if (getChar() == '/')
					return 1; 
			}
		} 
		return 1;
	}
	else{  //�ʷ�������������﷨�Ĵ���,���͵��﷨��������ʱ��֤��ȥ��token����ȷ�ԣ������token�ᱻ���Ե��� 
		ungetChar();
		return 0;
	}
} 

int yylex(){
	if (file == NULL)
		file = stdin;
//	Token token = NULL;	//�õ���ȫ�ֱ���token 
	int ch, ch2;  // ch2����˫�ֽ���������ж� 
	if (file == NULL)
		errExit("expect a text file");
	ch = getChar();
	while(ch == '\t' || ch == '\n' || ch == '\x20' || check_comment(ch)){
		// ��ʱ��ͳ���кź��к� 
		ch = getChar();
	}
	// ��ʱ�������»��߿�ͷ�ı�ʶ�� ���Լ��ַ���Խ������� 
	char buf[BUFLEN];
	char *p = buf;
	if (isalpha(ch)){  // ��ͷΪ��ĸ�����
		do{
			*p++ = ch; 
		
		} while(isalnum((ch = getChar())) && ch != EOF);
		*p = '\0';
		ungetChar();  // �����ַ������������� 
		int sym = keyword_lookup(buf); 
		token = sym == 0?newToken(buf,ID):newToken(buf,sym); 

	}
	else if (isdigit(ch)){
		ungetChar(); 
		int integer;
		// Ϊ�˼����ԣ�Ӧ��ʹ��fsanf :return ��������ĸ��� 
		//***�Ѿ�������   fscanf(file,"%d",&integer);
		integer = getNumber();
		//��ʽ�����ݣ���д�뵽�ַ������� 
		sprintf(buf,"%d",integer);
		token = newToken(buf,NUM); 
		//��token->integer��ֵ 
		token->integer = integer;	
	}
	else	
		switch(ch){
			case '=':{
				ch2 = getChar();
				if (ch2 == '=')
					token = newToken("==", EQ);
				else{
					ungetChar();
					token = newToken("=",'=');
				}  
				break;
			}
			case '<':{
				ch2 = getChar();
				if (ch2 == '=') 
					token = newToken("<=",LE);
				else{
					ungetChar();
					token = newToken("<",LT);
				}
				break;
			}
			case '>':{
				ch2 = getChar();
				if (ch2 == '=') 
					token = newToken(">=",GE);  // ���ڵ��� 
				else{
					ungetChar();
					token = newToken(">",GT);  // ���� 
				}
				break;
			}
			case '!':{
				ch2 = getChar();
				if (ch2 == '=')
					token = newToken("!=", NE);
				else{
					ungetChar();
					token = newToken("!",'!'); 
				} 
				break;
			}
			case EOF:
				token = newToken("",0);
				break;
			default:{
				char strs[2] = {ch, 0};
				token = newToken(strs, ch);
			}
		}

	return token->sym;
}
void errExit(char *msg){
	fprintf(stderr,"%s,�ھͽ���%d��,��%d��\n",msg,LINENUM,COLNUM);
	exit(1);
}

