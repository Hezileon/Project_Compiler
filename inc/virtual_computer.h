#pragma once
#ifndef VIRTUAL_COMPUTER_H
#define VIRTUAL_COMPUTER_H

#include <vector>
struct opxyz
{
	int x, y, z;

	opxyz(int _x = 0, int _y = 0, int _z = 0):x(_x),y(_y),z(_z){}
};
// to judge the type of op 
inline bool is_single_command(int x)
{
	return (0 <= x && x <= 14)||(x == 50)||(x == 60);
}
// to judge the type of op 
inline bool is_complex_command(int x)
{
	return (x == 20) || (x == 30) || (x == 40);
}
// to judge the type of op
inline bool is_binary_operator(int x)
{
	return (x == 1 || x == 2) || (4 <= x && x <= 13) || (x == 30) || (x == 40);
}
// to judge the type of op
inline bool is_receiving_single_token(int x)
{
	return( x == 60 )||( x == 50 );
}
class Command
{
private:
public:
	virtual const opxyz get_info() = 0;
	virtual int getOp() = 0;
	virtual void execute() = 0;
};
class Single_Command : public Command
{
private:
	int op;
	int x, y, z;
public:
	Single_Command(int _op, int _x, int _y, int _z):op(_op), x(_x), y(_y), z(_z) {}
	~Single_Command(){}
	virtual const opxyz get_info() override { return opxyz(x,y,z);}
	virtual int getOp() override { return  op; }
	virtual void execute() override;
};

class Complex_Command : public Command
{
private:
	int op;
	int N, x, cid;
	std::vector<Command*> CCOMMANDS;
public:
	Complex_Command(int _op, int _N, int _x, int _cid);
	Complex_Command(const Complex_Command& cCmd);
	~Complex_Command() {for(Command* ptr: CCOMMANDS){delete ptr;}}
	const Complex_Command & operator=(const Complex_Command& ccmd)
	{
		N = ccmd.N;
		x = ccmd.x;
		cid = ccmd.cid;

		for (Command* ptr : ccmd.CCOMMANDS)
		{
			opxyz info = ptr->get_info();
			Command* p = new Single_Command(ptr->getOp(), info.x, info.y, info.z);
			CCOMMANDS.push_back(p);
		}
		return *this;
	}
	virtual const opxyz get_info() override {return opxyz(N, x, cid);}
	virtual int getOp() override { return  op; }
	virtual void execute() override;
};



void Create_global_Complex_Command(Command*ptr, int cid);
void Virtual_Computer_Main();

#endif
