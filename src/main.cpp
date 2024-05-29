#include "../inc/Lexer.h"
#include <iostream>

#include "../inc/virtual_computer.h"

using namespace std;

// 三步走：1.正确输入词法单元（lexer可以正确使用） 2.递归处理语义 3.实现execute()

int main()
{
	Lexer obj;// the constructor of Lexer will do the job of input;

	for (Token123* p = obj.getToken(); p != nullptr; p = obj.getToken())
	{
		p->_Test_Show();
	}

	cout << "endofinput";
	return 0;
}
