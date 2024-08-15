#include "../inc/Lexer.h"
#include <iostream>
#include "../inc/virtual_computer_advanced.h"
//#include "../inc/virtual_computer.h"
#include "../inc/Expression.h"
#include "../inc/Statement.h"
#include "../inc/preprocessor.h"
using namespace std;

// 三步走：1.正确输入词法单元（lexer可以正确使用） 2.递归处理语义 3.实现execute()

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
