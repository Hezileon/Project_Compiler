#include "../inc/Lexer.h"
#include <iostream>
#include "../inc/virtual_computer_advanced.h"
//#include "../inc/virtual_computer.h"
#include "../inc/Expression.h"
#include "../inc/Statement.h"
using namespace std;

// 三步走：1.正确输入词法单元（lexer可以正确使用） 2.递归处理语义 3.实现execute()

int main()
{
	bool compilerModeOn = false;
	//bool compilerModeOn = true;
	if(compilerModeOn)
	{
		Lexer obj;// the constructor of Lexer will do the job of input;
		//obj._Test_Lexer_Check();
		seqStatement main = seqStatement{ &obj };
		main.execute();
	}
	else
	{
		Virtual_Computer_Advanced_Main();
	}
	return 0;
	

	
}
