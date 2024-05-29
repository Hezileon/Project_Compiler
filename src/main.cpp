#include "../inc/Lexer.h"
#include <iostream>

#include "../inc/virtual_computer.h"

using namespace std;

int main()
{
	Lexer obj;// the constructor of Lexer will do the job of input;

	for (Token123* p = obj.getToken(); p != nullptr; p = obj.getToken())
	{
		
	}

	cout << "endofinput";
	return 0;
}
