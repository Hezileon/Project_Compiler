#include "../inc/Lexer.h"
#include <iostream>
#include "../inc/virtual_computer.h"
#include "../inc/Expression.h"
using namespace std;

// 三步走：1.正确输入词法单元（lexer可以正确使用） 2.递归处理语义 3.实现execute()

int main()
{
	Lexer obj;// the constructor of Lexer will do the job of input;

	expression* exp = parseExpression(&obj);
	cout<<exp->evaluate();

	return 0;
}
