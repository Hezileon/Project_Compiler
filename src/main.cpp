#include "../inc/Lexer.h"
#include <iostream>
#include "../inc/virtual_computer.h"
#include "../inc/Expression.h"
#include "../inc/Statement.h"
using namespace std;

// �����ߣ�1.��ȷ����ʷ���Ԫ��lexer������ȷʹ�ã� 2.�ݹ鴦������ 3.ʵ��execute()

int main()
{
	Lexer obj;// the constructor of Lexer will do the job of input;
	// obj._Test_Lexer_Check();
	seqStatement main = seqStatement{ &obj };
	main.execute();

	//expression* temp = parseExpression(&obj);
	//cout << temp->evaluate();
	return 0;
}
