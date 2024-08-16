#pragma once
#ifndef PREPROCESSER_H_
#define PREPROCESSER_H_


/*
 *		if (compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << asm_num << " " << x << " " << y << " " << z;
			if (anotationModeOn) std::cout << "// ...";
			std::cout << std::endl;
		}
 */
#include <stack>

#include "compiler.h"
#include "../inc/Expression.h"
#include <cstdlib>

std::string Label_Allocator();

extern int stmCnt ;
extern bool showStatementModeOn;

class environment;
class block;

class statement
{
public:
	virtual void execute() = 0;
	virtual void show_yourself() = 0;
	//virtual int getLineCnt() = 0;
};

inline std::string funcName_generate(char* c, int psize) {
	std::string func = c;
	for (int i = 0; i < psize; i++) func = func + "i";
	return func;
}
inline int NameToAddress(char* c, environment* var_env);
class block
{
private:
	std::vector<statement*> stm;
	environment* myEnv;
	int pos;
	bool isFuncBlock;
	int parmSize;
	std::string myLabel;
	std::string rtnLabel;
	std::string skipLabel;
	// std::map<statement*, std::string> rtnLabel;// TODO-2

	// TODO-LIST

	// 0. label allocator;
	// 1. write generateCode();(generate the block of function, this code is a label version of code;)
	// 2. write code for all kind of statement and check their correctness; ---> the checking correctness part i would do it mannually
	// 3. modify processor.execute() to call generateCode();
	// 
	// 5.
	// 6. 
public:
	block(Lexer*,environment*,bool);
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

	void generateCode();
	std::string	getLabel_start() { return myLabel; }
	std::string getLabel_rtn() { return rtnLabel; }
	std::string getLabel_skip() { return skipLabel; }

	void setParmSize(int size_) { parmSize = size_; }
	// plz make sure the curEnv and curBlock are correct before you call this function;
	void executeAllStatement() { for (statement* s : stm) { s->execute(); } }

};
class environment
{
private:
	environment* f_env;
	bool isWhileBlock;
	expression* cond;
	int varCnt;

	int stk_addr;
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
		MEM[NameToAddress(c, this)] = value;
	}
	void createFunc(char* c, int psize);
	void updateDest(char* c, int psize, block* dest);
	char* getVarTbl(int i) { return varTbl_c[i]; }
	bool KeyValueFound(char* c)
	{
		std::string var = c;
		return Value.count(var);
	}
	int readValue(char* c) {
		std::string var = c;
		if (Value.count(var)) { return MEM[stk_addr-Value[var]]; }
	}
	int readAddr(char* c)
	{
		std::string var = c;
		if (Value.count(var)) { return stk_addr-Value[var]; }
		else { std::cerr << "readAddr_error, no key value found"<<std::endl; }
	}
	void setStackAddr(int s) { stk_addr = s; }

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
	

	std::vector<std::string> getGlobalFuncDef() { return funcTbl; }
	block* getFuncBlock(std::string name) { return Dest[name]; }
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

	bool statusOperating;
	block* MainBlock;
	// 1. processor.execute() -> modification!
	// 2. setMainBlock? happens at?
	// 3. if no MainBlock found, set MainBlock to curBlock

public:

	void push_JmpDest(block* x) { jmpDest_s.push(x); }
	void push_RtnDest(block* x) { rtnDest_s.push(x); }
	void push_RtnPos(int x) { rtnPos_s.push(x); }
	void pop_JmpDest(block* x) { jmpDest_s.pop(); }
	void pop_RtnDest(block* x) { rtnDest_s.pop(); }
	void pop_RtnSos(int x) { rtnPos_s.pop(); }
	block* read_JumpDest_Top() { return jmpDest_s.top(); }
	block* read_ReturnDest_Top() { return rtnDest_s.top(); }
	void main_block_init()
	{
		curBlock = MainBlock;
		curEnv = curBlock->getMyEnv();
		nextPos = 0;

		int offset = curBlock->getMyEnv()->getVarCnt() + 1;
		MEM[MEM[200] + 1] = MEM[200];
		MEM[200] = MEM[200] + offset;
		curEnv->setStackAddr(MEM[200]);
	}

	void jump();
	void jump_para(std::vector<expression*> parms);
	void Return();
	void Return_While();
	void execute();
	environment* getCurEnv() { return curEnv; }


	void setMainBlock(block* mainBlock_)
	{
		MainBlock = mainBlock_;
	}
	block* getMainBlock() { return MainBlock; }
	block* getCurBlock() { return curBlock; }
	preprocessor(Lexer* lexer);
	~preprocessor(){}


	void setCurBlockandEnv(block* curB_) { curBlock = curB_; curEnv = curB_->getMyEnv(); }
	void setNextPos(int i) { nextPos = i; }
};

extern preprocessor processor;

class assignment : public statement
{
private:
	char* idf;
	char* op;
	expression* exp;
	block* myBlock;
public:
	assignment(char* _idf, char* _op, expression* _exp,block* myBlock_);

	void execute() override;
	void show_yourself() override { std::cout << stmCnt++ << ": assignment" << std::endl; }
	~assignment();
	// int getLineCnt() override{}
};

class output : public statement
{
private:
	char* op;
	expression* exp;
	block* myBlock;
public:
	output(char* _op, expression* exp,block* myB_);

	void execute() override;
	void show_yourself() override { std::cout << stmCnt++ << ": output" << std::endl; }
	~output();
};

class input : public statement
{
private:
	char* varName;
	block* myBlock;
public:
	input(char* n,block* myBlock_) :varName(n),myBlock(myBlock_) { std::cout << stmCnt++ << ": input" << std::endl; }
	~input() {}

	void execute() override;
	
	void show_yourself() override{ std::cout << stmCnt++ << ": input" << std::endl; }
	//virtual int getLineCnt() override{}
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
		// QUESTION-1
	}
	void show_yourself() override { std::cout << stmCnt++ << ": function Def" << std::endl; }
	~functionDef() {}
	//virtual int getLineCnt() override{}
};


// just take care of pushing into stack;
// though in fact no operation is implemented directly, it still has to exist!

class varDecl : public statement
{
private:
	char* var;
	environment* myEnv;
public:
	varDecl(char* c, environment* myEnv_);
	~varDecl() = default;

	void execute() override;
	void show_yourself() override { std::cout << stmCnt++ << ": variable declaration" << std::endl; }
	// virtual int getLineCnt() override{}
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

	void execute();
	
	void show_yourself() override { std::cout << stmCnt++ << ": function call" << std::endl; }
	~functionCall() {}
	//virtual int getLineCnt() override{}
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
	void show_yourself() override { std::cout << stmCnt++ << ": if statement" << std::endl; }
	~ifStatement() {}
	//virtual int getLineCnt() override{}
};

class returnStatement:public statement
{
private:
	bool isWhileVersion;
	expression* cond;
	block* myBlock;
public:
	returnStatement(block* myBlock_):myBlock(myBlock) { isWhileVersion = false; cond = nullptr; }
	returnStatement(expression* cd,block* myBlock_) :cond(cd),myBlock(myBlock_) { isWhileVersion = true; }
	~returnStatement() = default;

	void execute() override;
	void show_yourself() override { std::cout << stmCnt++ << ": return statement ( no rtn value)" << std::endl; }
};

class returnValueStatement :public statement
{
private:
	expression* exp;
	block* myBlock;
public:
	returnValueStatement(expression* exp_,block* myBlock_):exp (exp_),myBlock(myBlock_) {  }
	~returnValueStatement() = default;

	void execute() override;
	
	void show_yourself() override { std::cout << stmCnt++ << ": return with value statement" << std::endl; }
};
// TODO-2
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
	void show_yourself() override { std::cout << stmCnt++ << ": while statement" << std::endl; }

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
	void show_yourself() override {}
	~seqStatement();
};

#endif
