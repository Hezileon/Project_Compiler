#include "../inc/Lexer.h"
#include <iostream>
#include "../inc/virtual_computer_advanced.h"
//#include "../inc/virtual_computer.h"
#include "../inc/Expression.h"
#include "../inc/Statement.h"
#include "../inc/preprocessor.h"
using namespace std;

// �����ߣ�1.��ȷ����ʷ���Ԫ��lexer������ȷʹ�ã� 2.�ݹ鴦������ 3.ʵ��execute()

Lexer obj;
extern preprocessor processor = preprocessor{ &obj };

int main()
{
	// bool compilerModeOn = false;
	bool compilerModeOn = true;
	if(compilerModeOn)
	{
		/*
		Lexer obj;// the constructor of Lexer will do the job of input;
		//obj._Test_Lexer_Check();
		seqStatement main = seqStatement{ &obj };
		main.execute();
		*/
		
		processor.execute();
	}
	else
	{
		Virtual_Computer_Advanced_Main();
	}
	return 0;
	

	
}
