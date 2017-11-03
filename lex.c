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

// file 变量定义在lex.h头文件中 
int LINENUM = 1;                        // 行号 
int COLNUM = 0;                         // 列号 
char buff[BUFLEN];                      // 从文件中读取的字符的数目
char *pos = buff + BUFLEN;          // 指向当前
int size = 0;                               //当前缓冲区读入字符的 
int lastChar;
Token token;						//　为语法文件cmm.y准备token 
FILE *file = NULL;

// 用于检查是否为关键字的关键字到sym映射库，减少多重判断 
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

//用于获取下一个字符，并来统计当前所读字符所在的行号和列号 
char getChar(){
	if (pos == buff + BUFLEN ){  //表示缓冲区已经数据已经使用完毕了，需要重新读取文件到缓冲区中 
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
	 
	return EOF;  // 文件数据读完，将返回-1 
}
//将已读字符退回到缓冲区中 
void ungetChar(){
	pos--; // 指针回退，表示将字符协会 
	lastChar = *(pos - 1);	   // 上一个字符也要跟着回退 
	//如果当前指针所指向的字符为‘\n’为保证行号的正确性，LINENUM应减去1
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
	while(strcmp(p -> ID,ID) != 0 && strcmp(p -> ID,"NULL") != 0) // NON，NULL待统一 
		p ++;
	return p -> sym; 	
}

Token newToken(char *ID,int sym){
	Token token = (Token)malloc(sizeof(*token));
	token->ID = strdup(ID);
	token->sym = sym;
	return token;
}

// 检查注释，将注释当做空白字符来处理 
int check_comment(int ch){
	if (ch != '/')
	 	return 0;  //0代表为不用过滤
	char ch2 = getChar(); 
	if (ch2 == '/'){  // 得知为单行注释，对所在行出现的任何字符做跳过处理  
		 
		 while((ch2 = getChar()) != EOF && ch2 != '\n');
		  return 1;
	}
	else if (ch2 == '*'){  //检查得知为多行注释 
		for (ch = getChar(); ch != EOF; ch = getChar()){
			if (ch == '*'){  //检查到‘*’多行注释，进入检查结束状态 
				if (getChar() == '/')
					return 1; 
			}
		} 
		return 1;
	}
	else{  //词法分析器不检查语法的错误,但送到语法分析器中时保证送去的token的正确性（错误的token会被忽略掉） 
		ungetChar();
		return 0;
	}
} 

int yylex(){
	if (file == NULL)
		file = stdin;
//	Token token = NULL;	//用的是全局变量token 
	int ch, ch2;  // ch2用于双字节运算符的判断 
	if (file == NULL)
		errExit("expect a text file");
	ch = getChar();
	while(ch == '\t' || ch == '\n' || ch == '\x20' || check_comment(ch)){
		// 暂时不统计行号和列号 
		ch = getChar();
	}
	// 暂时不考虑下划线开头的标识符 ：以及字符数越界等问题 
	char buf[BUFLEN];
	char *p = buf;
	if (isalpha(ch)){  // 开头为字母的情况
		do{
			*p++ = ch; 
		
		} while(isalnum((ch = getChar())) && ch != EOF);
		*p = '\0';
		ungetChar();  // 回退字符串到输入流中 
		int sym = keyword_lookup(buf); 
		token = sym == 0?newToken(buf,ID):newToken(buf,sym); 

	}
	else if (isdigit(ch)){
		ungetChar(); 
		int integer;
		// 为了兼容性，应该使用fsanf :return 读入参数的个数 
		//***已经不兼容   fscanf(file,"%d",&integer);
		integer = getNumber();
		//格式化数据，并写入到字符串当中 
		sprintf(buf,"%d",integer);
		token = newToken(buf,NUM); 
		//给token->integer赋值 
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
					token = newToken(">=",GE);  // 大于等于 
				else{
					ungetChar();
					token = newToken(">",GT);  // 大于 
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
	fprintf(stderr,"%s,在就近第%d行,第%d列\n",msg,LINENUM,COLNUM);
	exit(1);
}

