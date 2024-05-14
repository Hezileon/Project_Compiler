#include "../inc/Lexer.h"
#include <iostream>

using namespace std;

int main()
{
	Lexer obj;
	//obj._Test_Lexer_Check();

	for(Token123* p = obj.getToken(); p != nullptr ; p = obj.getToken())
	{
		p->_Test_Show();
	}
	cout << "endofinput";

	return 0;
}