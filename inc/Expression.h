#pragma once
#ifndef EXPRESSION_H_
#define EXPRESSION_H_
#include <map>

#include "Lexer.h"
extern std::map<std::string, int> IdfToValue;
int findIdfValue(char* str);
int assignValueToIdf(char* _str, int value);

enum outcome_type
{
	outcome_int,
	outcome_str,
	outcome_bool
};

union outcome_value
{
	int val;
	char* str;
	bool trueOrFalse;

	outcome_value(int _v) :val(_v) {}
	outcome_value(char* _str) : str(_str) {}
	outcome_value(bool _trueOrFalse) : trueOrFalse(_trueOrFalse) {}
};


struct outcome
{
	outcome_type myType;
	outcome_value myValue;
	outcome(int _type,int _v): myValue(_v)
	{
		if (_type == 0) { myType = outcome_int; }
		else if (_type == 1) { myType = outcome_str; }
		else if (_type == 2) { myType = outcome_bool; }
	}
	outcome(int _type, char* str) : myValue(str)
	{
		if (_type == 0) { myType = outcome_int; }
		else if (_type == 1) { myType = outcome_str; }
		else if (_type == 2) { myType = outcome_bool; }
	}
	outcome(int _type, bool b) : myValue(b)
	{
		if (_type == 0) { myType = outcome_int; }
		else if (_type == 1) { myType = outcome_str; }
		else if (_type == 2) { myType = outcome_bool; }
	}
};
class expression
{
private:
public:
	virtual int evaluate() = 0;
	virtual void evaluate_compiler(int pos) = 0;
};

class expressionTernery : public expression
{
private:
	expression* exp_1;
	expression* exp_2;
	expression* exp_3;
public:
	expressionTernery(expression* _exp_1, expression* _exp_2, expression* _exp_3);

	virtual int evaluate() override;
	virtual void evaluate_compiler(int pos) override;
	~expressionTernery();
};

class expressionBinary : public expression
{
	char* op;
	expression* exp_1;
	expression* exp_2;
public:
	expressionBinary(expression* _exp_1, expression* _exp_2, char* _op);

	virtual int evaluate() override;
	virtual void evaluate_compiler(int pos) override;
	~expressionBinary();
};

class expressionUnary : public expression
{
	char* op;
	expression* exp_1;
public:
	expressionUnary(expression* _exp_1,char* _op);
	virtual int evaluate() override;
	virtual void evaluate_compiler(int pos) override;
	~expressionUnary();
};

class expressionBasic : public expression
{
	enum
	{
		basicType_idf,
		basicType_int,
		basicType_exp
	}basicType;
	union variable
	{
		char* Idf;
		int Int;
		expression* exp;
	} var;

public:
	expressionBasic(Lexer* lexer);
	expressionBasic(int typeNum, Lexer);// if encounter "("

	virtual int evaluate() override;
	virtual void evaluate_compiler(int pos) override;
};

expression* parseExpression(Lexer* lexer);
expression* parseExpressionInBracket(Lexer* lexer,bool leftBracketAlreadyIncluded = true);
expression* parseExpression1(Lexer* lexer);
expression* parseExpression2(Lexer* lexer);
expression* parseExpression3(Lexer* lexer);
expression* parseExpression4(Lexer* lexer);

#endif
