#ifndef LEX_H_
#define LEX_H_
extern FILE *file;
typedef enum{
	NON, LP, RP, LC, RC, COMMA, SEMI, ASSIGN, ID, NUM, INT,  // �գ���Բ���ţ���Բ���ţ������ţ��һ�����,���ţ��ֺţ����ںţ������������γ����� ����
	IF, ELSE, WHILE, PRINT, RETURN, READ, NOT,  // READ ͬscanf
	ADD, SUB, MUL, DIV, LT, LE, GT, GE, EQ, NE,  // +, -, *, /, >, >=, <, <=, ==, != 
	END 
}SYMBOL;

 typedef struct token{
	char *ID;
	SYMBOL sym;
	int integer;
} *Token;

Token getToken();
void errExit(char *msg);
void setSource(FILE *_file);

#endif

