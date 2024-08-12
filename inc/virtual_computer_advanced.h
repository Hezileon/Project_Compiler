#pragma once
#ifndef VIRTUAL_COMPUTER_ADVANCED_H
#define VIRTUAL_COMPUTER_ADVANCED_H
#include <string>

inline bool isCommand(int x){return (0 <= x && x <= 14) || (x == 50) || (x == 60) || (x == 20) || (x == 30) || (x == 40);}

// to judge the type of op
inline bool isBinaryOperator(int x)
{
	return (x == 1 || x == 2) || (4 <= x && x <= 13);
}
inline bool isUnaryOperatorX(int x)
{
	return (x == 60) || (x == 50) ;
}
inline bool isUnaryOperatorZ(int x)
{
	return (x == 30) || (x == 40);
}
inline bool isControlOperator(int x)
{
	return (x == 20) || (x == 30) || (x == 40);
}
class Cmd
{
private:
	int op;
	int x, y, z;
public:
	Cmd(int _op, int _x, int _y, int _z)
	{
		op = _op;
		z = _z;
		y = _y;
		x = _x;
	}

	int getop() { return op; }
	int getx() { return x; }
	int gety() { return y; }
	int getz() { return z; }

	void execute();
};
void Virtual_Computer_Advanced_Main();


#endif