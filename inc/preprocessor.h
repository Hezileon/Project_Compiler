#pragma once
#ifndef PREPROCESSER_H_
#define PREPROCESSER_H_

#include <stack>

#include "compiler.h"
#include "Statement.h"
#include "../inc/Expression.h"


class environment;
class block;
class statement
{

public:
	virtual void execute() = 0;
};

inline std::string funcName_generate(char* c, int psize) {
	std::string func = c;
	for (int i = 0; i < psize; i++) func = func + "i";
	return func;
}
inline int NameToAddress(char* c);
class block
{
private:
	std::vector<statement*> stm;
	std::vector<block*> jumpTo; //?
	environment* myEnv;
	int pos;
	// int Tag;

public:
	block(Lexer*,environment*);
	~block() = default;

	statement* getNextStatement()
	{
		if (pos < stm.size()) { return stm[pos++]; }
		else { return nullptr; }
	}
	void setPos(int nextPos) { pos = nextPos; }
	environment* getMyEnv() { return myEnv; }
	int readPos(statement* ptr)
	{
		int pos = 0;
		for (pos; pos < stm.size(); pos++)
		{
			if (ptr == stm[pos]) { return pos + 1; }
		}
		return 0;
	}
	int getCurPos()
	{
		return pos;
	}
	void pushStatement(statement* s) { stm.push_back(s); }
};

class preprocessor
{
private:
	static int TagCnt;
	std::stack <block*> jmpDest_s;
	std::stack <block*> rtnDest_s;
	std::stack<int> rtnPos_s;

	environment* curEnv;
	block* curBlock;
	int nextPos;
	statement* nextStatement;




public:

	void push_JmpDest(block* x) { jmpDest_s.push(x); }
	void push_RtnDest(block* x) { rtnDest_s.push(x); }
	void push_RtnPos(int x) { rtnPos_s.push(x); }
	void pop_JmpDest(block* x) { jmpDest_s.pop(); }
	void pop_RtnDest(block* x) { rtnDest_s.pop(); }
	void pop_RtnSos(int x) { rtnPos_s.pop(); }

	void jump()
	{
		curBlock = jmpDest_s.top();
		nextPos = 0;
	}
	void Return()
	{
		curBlock = rtnDest_s.top();
		nextPos = rtnPos_s.top();

		jmpDest_s.pop();
		rtnDest_s.pop();
		rtnPos_s.pop();
	}
	void Return_While()
	{
		nextPos = 0;
	}
	void execute()
	{
		// MAIN_PROCESS_LOOP
		MEM[200] = 200;

		curBlock->setPos(nextPos++);
		statement* ptr = curBlock->getNextStatement();
		while(ptr != nullptr)
		{
			ptr->execute();
			curBlock->setPos(nextPos++); ptr = curBlock->getNextStatement();
		}
	}

	environment* getCurEnv() { return curEnv; }

	preprocessor(Lexer* lexer);
	~preprocessor(){}
};

extern preprocessor processor;

class environment
{
private:
	environment* f_env;
	bool isWhileBlock;
	expression* cond;
	int varCnt;

	int offsetAllo;

	std::vector<std::string> varTbl;
	std::vector<std::string> funcTbl;
	std::map<std::string, int> Value; 
	std::map<std::string, block*> Dest;

public:
	environment(environment* fE) : f_env(fE), isWhileBlock(false), cond(nullptr) { offsetAllo = 0; }
	environment(environment* fE,expression* cd) : f_env(fE),isWhileBlock(true),cond(cd) { offsetAllo = 0; }
	~environment(){}

	// use this when we are preprocessing the code, not when executing. And plz make sure you use the member function;
	void createVar(char* c)
	{
		std::string var = c; if (!Value.count(var)) { varTbl.push_back(var); Value[var] = offsetAllo++; varCnt++; }
	}
	void updateValue(char* c, int value)
	{
		std::string var = c; MEM[Value[var]] = value;
	}
	void createFunc(char* c, int psize)
	{
		std::string func = funcName_generate(c, psize);
		if (!Dest.count(func)) { funcTbl.push_back(func); Dest[func] = nullptr; }
	}
	void updateDest(char* c, int psize, block* dest) {
		std::string func = funcName_generate(c, psize); Dest[func] = dest; }
	bool KeyValueFound(char* c)
	{
		std::string var = c;
		return Value.count(var);
	} // TODO-1 realize The search order of multiple env; -> look into expression
	int readValue(char* c) {
		std::string var = c;
		if (Value.count(var)) { return MEM[Value[var]]; }
	}
	int readAddr(char* c)
	{
		std::string var = c;
		if (Value.count(var)) { return Value[var]; }
	}
	block* readDest(char* c, int pSize)
	{
		std::string func = funcName_generate(c, pSize); if (Dest.count(func)) { return Dest[func]; }
		else { std::cout << "Key value doesn't exist;" << std::endl; return nullptr; }
	}
	block* findFuncDest(char* c, int psize)
	{
		std::string func = funcName_generate(c, psize);
		block* rtn = nullptr;
		if (Dest.count(func) == 0) { if (f_env != nullptr) rtn = f_env->findFuncDest(c, psize); }
		else { rtn = Dest[func]; }
		return rtn;
	}
	bool ifIsWhileBlock() { return isWhileBlock; }
	expression* getCond() { return cond; }
	int getVarCnt() { return varCnt; }
	bool isGlobalEnv() { return f_env == nullptr;}
	environment* getFEnv() { return f_env; }
	
};



class assignment : public statement
{
private:
	char* idf;
	char* op;
	expression* exp;
public:
	assignment(char* _idf, char* _op, expression* _exp);

	void execute() override;

	~assignment();
};

class output : public statement
{
private:
	char* op;
	expression* exp;

public:
	output(char* _op, expression* exp);

	void execute() override;

	~output();
};

class input : public statement
{
private:
	char* varName;
public:
	input(char* n) :varName(n) {}
	~input() {}

	void execute() override
	{
		std::cin >> MEM[NameToAddress(varName)];
	}
};
// this should happen within preprocessor, because that its "execute()" is meaningless;

class functionDef : public statement
{
private:
	char* funcName;
	std::vector<char*> parameters;
	int pSize;
public:
	functionDef(char* _name) :funcName(_name) { pSize = 0; }
	void createPara(char* name) { parameters.push_back(name); pSize++; }
	int getPSize() { return pSize; }

	void execute() override
	{
		 //TODO
	}

	~functionDef() {}
};


// just take care of pushing into stack;
// though in fact no operation is implemented directly, it still has to exist!

class varDecl : public statement
{
private:
	char* var;
	environment* myEnv;
public:
	varDecl(char* c,environment* myEnv_):var(c),myEnv(myEnv_){}
	~varDecl() = default;

	void execute() override;
};

class functionCall : public statement
{
private:
	char* funcName;
	int pSize;
	std::vector<int> parms;
	block* myBlock;
public:
	functionCall(char* name, int _psize, block* _block) :funcName(name), pSize(_psize), myBlock(_block) {}
	void addParms(int v) { parms.push_back(v); }

	void execute()
	{
		block* dest = myBlock->getMyEnv()->findFuncDest(funcName, pSize);
		if (dest != nullptr)
		{
			processor.push_JmpDest(dest);
			processor.push_RtnDest(myBlock);
			processor.push_RtnPos(myBlock->readPos(this));
			processor.jump();
		}

	}
	~functionCall() {}
};

class ifStatement : public statement
{
private:
	expression* cond;
	block* myBlock; // the env that ifStatement is in;

	block* ifBlock;
	block* elseBlock;
public:
	ifStatement(expression* _cond, block* _block, block* ifB, block* elseB) :cond(_cond), myBlock(_block), ifBlock(ifB), elseBlock(elseB)
	{
	}

	void execute() override;
	~ifStatement() {}
};

class jumpStatement:public statement
{
private:
public:
	jumpStatement() = default;
	~jumpStatement() = default;

	void execute() override
	{
		processor.jump();
	}
};
// useless
class condJumpStatement :public statement
{
private:
	expression* cond;
public:
	condJumpStatement(expression* cond):cond(cond){}
	~condJumpStatement() = default;

	void execute() override
	{
		cond->evaluate_compiler(0);
		if (MEM[0])
		{
			processor.jump();
		}
	}
};

class returnStatement:public statement
{
private:
	bool isWhileVersion;
	expression* cond;
public:
	returnStatement() { isWhileVersion = false; cond = nullptr; }
	returnStatement(expression* cd) :cond(cd) { isWhileVersion = true; }
	~returnStatement() = default;

	void execute() override
	{
		if(isWhileVersion)
		{
			cond->evaluate_compiler(0);
			if (MEM[0])
			{
				processor.Return_While();
			}
			else
			{
				processor.Return();
			}
		}
		else
		{
			processor.Return();
		}
	}
};

class whileStatement :public statement
{
private:
	expression* cond;
	block* myBlock;
	block* whileBlock;
public:
	whileStatement(expression* _cond,block* myB, block* wb) :cond(_cond),myBlock(myB), whileBlock(wb) {}
	~whileStatement() {}

	void execute() override;
	

};
// derived class of class statement, containing "seq" to point at the other type of statement;
class seqStatement : public statement
{
private:
	std::vector<statement*> seq;

public:
	seqStatement(Lexer* lexer); // constructor
	seqStatement(statement* firstStatement);

	void addStatementToTail(statement* newStatement) {} // modify

	void execute() override;
	~seqStatement();
};

#endif
