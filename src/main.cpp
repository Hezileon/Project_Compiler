#include "../inc/Lexer.h"
#include <iostream>

using namespace std;

int main()
{
	Idf test_idf("123");
	test_idf.check();

	Op test_op('+');
	test_op.check();
	cout << endl << test_op.getOp();

	Int test_int(123);
	test_int.check();
	cout << endl <<test_int.getInt();


	return 0;
}