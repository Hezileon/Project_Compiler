#pragma warning(disable:4996)
#pragma once
#ifndef LEXER_H_
#define LEXER_H_

#include <iostream>
#include <cstring>
#include <vector>

enum Type
{
	Identifier = 0,
	Integer,
	Operator
};

class Token123
{
private:
	Type my_type;
public:
	Token123(int _type = 0)
	{
		if (_type == 0) { my_type = Identifier; }
		else if (_type == 1) { my_type = Integer; }
		else if (_type == 2) { my_type = Operator; }
	}
	const Type & getType() const { return my_type; }

	virtual const char* getIdf() = 0;
	virtual int getInt() = 0;
	virtual char getOp() = 0;

	virtual void _Test_Show()
	{
		if(my_type == Identifier)
		{
			std::cout << "identifier " << getIdf()<<std::endl;
		}
		else if(my_type == Integer)
		{
			std::cout << "integer " << getInt()<<std::endl;
		}
		else
		{
			std::cout << getOp()<<std::endl;
		}
	}
};

class Idf : public Token123
{
private:
	int len;
	char* str;
public:
	Idf(const char* ch,int _type = 0):Token123(_type)
	{
		len = strlen(ch);
		str = new char[len + 1];
		strcpy(str, ch);
	}
	void check()
	{
		std::cout << "My Type is:" << getType() << std::endl;
		std::cout << "My Value is:" << str << std::endl;
	}

	virtual const char* getIdf() override
	{
		return str;
	}

	int getInt() override
	{
		return 0;
	}
	char getOp() override
	{
		return '\0';
	}

	~Idf()
	{
		delete[] str;
	}
};

class Int : public Token123
{
private:
	int I;
public:
	Int(const int _x, int _type = 1) :Token123(_type)
	{
		I = _x;
	}
	void check()
	{
		std::cout << "My Type is:" << getType() << std::endl;
		std::cout << "My Value is:" << I << std::endl;
	}

	virtual int getInt() override
	{
		return I;
	}


	const char* getIdf() override{
		return nullptr;
	}
	char getOp() override{
		return '\0';
	}
};

class Op : public Token123
{
private:
	char OP;
public:
	Op(const char _op,int _type = 2) :Token123(_type)
	{
		OP = _op;
	}
	void check()
	{
		std::cout << "My Type is:" << getType() << std::endl;
		std::cout << "My Value is:" << OP << std::endl;
	}

	virtual char getOp() override
	{
		return OP;
	}


	const char* getIdf() override{ return nullptr;}
	int getInt() override{ return 0;}
};



class Lexer
{
private:
	std::vector<Token123*> Tokens;
	int reading_to;
public:
	Lexer();

	Token123* getToken();
	void reset_reading();
	void _Test_Lexer_Check()
	{
		for(Token123* p: Tokens)
		{
			p->_Test_Show();
		}
		std::cout << "endofinput";
	}
	~Lexer();
};


#endif
