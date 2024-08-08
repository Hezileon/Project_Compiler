#pragma once
#ifndef STATEMENT_H_
#define STATEMENT_H_
#include "Lexer.h"

class expression;

class statement
{

public:
	virtual int execute() = 0;
};

class assignment : public statement
{
private:
	char* idf;
	char* op;
	expression* exp;
public:
	assignment(char* _idf, char* _op, expression* _exp);

	int execute() override;

	~assignment();
};

class output : public statement
{
private:
	char* op;
	char* idf;

public:
	output(char* _op, char* _idf);

	int execute() override;

	~output();
};

// derived class of class statement, containing "seq" to point at the other type of statement;
class seqStatement : public statement
{
private:
	std::vector<statement*> seq;

public:
	seqStatement(Lexer* lexer); // constructor
	seqStatement(statement* firstStatement);

	void addStatementToTail(statement* newStatement); // modify

	int execute() override;
	~seqStatement();
};

#endif
