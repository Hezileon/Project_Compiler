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

// abstract base class
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

	// These methods, having access to the private member of the class, are unsafe to use; 
	virtual const char* getIdf_privateValue() = 0;
	virtual int getInt_privateValue() = 0;
	virtual char getOp_privateValue() = 0;

	// covariant + copy function... This may seems to be a good idea?
	virtual Token123& getPtr() = 0;


	virtual void _Test_Show()
	{
		if(my_type == Identifier)
		{
			std::cout << "identifier " << getIdf_privateValue()<<std::endl;
		}
		else if(my_type == Integer)
		{
			std::cout << "integer " << getInt_privateValue()<<std::endl;
		}
		else
		{
			std::cout << getOp_privateValue()<<std::endl;
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
	virtual Idf& getPtr() override { return *this; }
	Idf getIdf() { return Idf{ this->str, this->len }; }


	// add copy member function
	Idf(const Idf&);
	Idf& operator=(const Idf& identifier);
	bool operator==(const char* ch) const;

	void check()
	{
		std::cout << "My Type is:" << getType() << std::endl;
		std::cout << "My Value is:" << str << std::endl;
	}

	virtual const char* getIdf_privateValue() override
	{
		return str;
	}

	int getInt_privateValue() override
	{
		return 0;
	}
	char getOp_privateValue() override
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
	virtual Int& getPtr() override { return *this; }
	Int getInt() { return Int{I}; }

	Int(const Int&);
	Int& operator=(const Int&);

	void check()
	{
		std::cout << "My Type is:" << getType() << std::endl;
		std::cout << "My Value is:" << I << std::endl;
	}

	virtual int getInt_privateValue() override
	{
		return I;
	}


	const char* getIdf_privateValue() override{
		return nullptr;
	}
	char getOp_privateValue() override{
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
	virtual Op& getPtr() override { return *this; }
	Op getOp() { return Op{ OP }; }

	Op(const Op&);
	Op& operator=(const Op&);


	void check()
	{
		std::cout << "My Type is:" << getType() << std::endl;
		std::cout << "My Value is:" << OP << std::endl;
	}

	virtual char getOp_privateValue() override
	{
		return OP;
	}


	const char* getIdf_privateValue() override{ return nullptr;}
	int getInt_privateValue() override{ return 0;}
};

// storing tokens as a whole, use getToken() to get next token in the lexer;
class Lexer
{
private:
	std::vector<Token123*> Tokens;
	int reading_to;
public:
	Lexer();
	// WARNING! MISSING copy_constructor and operator= ;

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
