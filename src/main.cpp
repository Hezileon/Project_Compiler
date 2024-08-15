#include "../inc/Lexer.h"
#include <iostream>
#include "../inc/virtual_computer_advanced.h"
//#include "../inc/virtual_computer.h"
#include "../inc/Expression.h"
#include "../inc/Statement.h"
#include "../inc/preprocessor.h"
using namespace std;

std::map<std::string, int> varToAddress;
extern int global_var_cnt = 0;

// �����ߣ�1.��ȷ����ʷ���Ԫ��lexer������ȷʹ�ã� 2.�ݹ鴦������ 3.ʵ��execute()
extern bool anotationModeOn = false;
extern bool lineCounterModeOn = false;
extern bool compileModeOn = false;

extern bool showStatementModeOn = true;
extern bool output_banned = false;
extern int LC;
extern int stmCnt = 0;

Lexer obj;
extern preprocessor processor = preprocessor{ &obj };

int main()
{
		
	/*
		Lexer obj;// the constructor of Lexer will do the job of input;
		//obj._Test_Lexer_Check();
		seqStatement main = seqStatement{ &obj };
		main.execute();
		*/
	processor.execute();
	return 0;
	

	
}
