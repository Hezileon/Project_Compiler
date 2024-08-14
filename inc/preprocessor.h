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
class environment
{
private:
	environment* f_env;
	bool isWhileBlock;
	expression* cond;
	int varCnt;

	int offsetAllo;

	std::vector<char* > varTbl_c;
	std::vector<std::string> varTbl;
	std::vector<std::string> funcTbl;
	std::map<std::string, int> Value;
	std::map<std::string, block*> Dest;

public:
	environment(environment* fE) : f_env(fE), isWhileBlock(false), cond(nullptr) { offsetAllo = 0; }
	environment(environment* fE, expression* cd) : f_env(fE), isWhileBlock(true), cond(cd) { offsetAllo = 0; }
	~environment() {}

	// use this when we are preprocessing the code, not when executing. And plz make sure you use the member function;
	void createVar(char* c)
	{
		varTbl_c.push_back(c);
		std::string var = c; if (!Value.count(var)) { varTbl.push_back(var); Value[var] = offsetAllo++; varCnt++; }
	}
	void updateValue(char* c, int value)
	{
		MEM[NameToAddress(c)] = value;
	}
	void createFunc(char* c, int psize)
	{
		std::string shortName = c;
		std::string func = funcName_generate(c, psize);
		if (!Dest.count(func)) { funcTbl.push_back(shortName); Dest[func] = nullptr; }
	}
	void updateDest(char* c, int psize, block* dest) {
		std::string func = funcName_generate(c, psize); Dest[func] = dest;
	}
	char* getVarTbl(int i) { return varTbl_c[i]; }
	bool KeyValueFound(char* c)
	{
		std::string var = c;
		return Value.count(var);
	} // TODO-1 realize The search order of multiple env; -> look into expression
	int readValue(char* c) {
		std::string var = c;
		if (Value.count(var)) { return MEM[MEM[200]-Value[var]]; }
	}
	int readAddr(char* c)
	{
		std::string var = c;
		if (Value.count(var)) { return MEM[200]-Value[var]; }
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
	bool isFuncCall(char* c)
	{
		std::string shortName = c;
		if (f_env == nullptr)
		{
			for (std::string n : funcTbl)
			{
				if (n == shortName) { return true; }
			}
			return false;
		}
		else
		{
			for (std::string n : funcTbl)
			{
				if (n == shortName) { return true; }
			}
			return f_env->isFuncCall(c);
		}
	}
	bool ifIsWhileBlock() { return isWhileBlock; }
	expression* getCond() { return cond; }
	int getVarCnt() { return varCnt; }
	bool isGlobalEnv() { return f_env == nullptr; }
	environment* getFEnv() { return f_env; }

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
		curEnv = curBlock->getMyEnv();
		nextPos = 0;

		int offset = curBlock->getMyEnv()->getVarCnt()+1;
		MEM[MEM[200] + 1] = MEM[200];
		MEM[200] = MEM[200] + offset;
	}

	void jump_para(std::vector<expression*> parms)
	{
		std::vector<int> p_answer;
		// use the parameters to calculate the answer and save them
		for(int i = 0 ;i < parms.size();i++ )
		{	
			parms[i]->evaluate_compiler(0);
			p_answer.push_back(MEM[0]);
		}



		curBlock = jmpDest_s.top();
		curEnv = curBlock->getMyEnv();
		nextPos = 0;

		int offset = curBlock->getMyEnv()->getVarCnt() + 1;
		MEM[MEM[200] + 1] = MEM[200];
		MEM[200] = MEM[200] + offset;

		for (int i = 0; i < parms.size(); i++)
		{
			char* var_c = curEnv->getVarTbl(i);// bind value to variables -> get the name of the variables;

			MEM[NameToAddress(var_c)] = p_answer[i]; // notice that the expression should be calculated before entering the block,
													// while the variable should be assigned after entering the block;
		}
	}
	void Return()
	{
		int offset = curBlock->getMyEnv()->getVarCnt()+1;
		MEM[200] = MEM[MEM[200] - offset+1];

		curBlock = rtnDest_s.top();
		curEnv = curBlock->getMyEnv();
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
	std::vector<expression*> parms;
	block* myBlock;
public:
	functionCall(char* name, int _psize, block* myBlock_) :funcName(name), pSize(_psize), myBlock(myBlock_) {}
	void addParms(expression* exp_) { parms.push_back(exp_); }

	void execute()
	{
		block* dest = myBlock->getMyEnv()->findFuncDest(funcName, pSize);
		if (dest != nullptr)
		{
			processor.push_JmpDest(dest);
			processor.push_RtnDest(myBlock);
			processor.push_RtnPos(myBlock->readPos(this));
			
			processor.jump_para(parms);
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
class returnValueStatement :public statement
{
private:
	expression* exp;

public:
	returnValueStatement(expression* exp_):exp (exp_) {  }
	~returnValueStatement() = default;

	void execute() override
	{
		exp->evaluate_compiler(0);

		MEM[100] = MEM[0];

		processor.Return();
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
