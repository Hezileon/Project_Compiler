#include "../inc/Lexer.h"
#include <iostream>

#include "../inc/virtual_computer.h"

using namespace std;

// �����ߣ�1.��ȷ����ʷ���Ԫ��lexer������ȷʹ�ã� 2.�ݹ鴦������ 3.ʵ��execute()

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
