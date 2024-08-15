#include "../inc/Statement.h"
#include "../inc/Expression.h"
#include "../inc/compiler.h"
extern int MEM[500];
extern int LC;

std::map<std::string, int> varToAddress;
extern int global_var_cnt = 0;

extern bool anotationModeOn;
extern bool lineCounterModeOn;
extern bool compileModeOn;

int _nameToAddress(environment* env, char* c)
{
	if (env->isGlobalEnv())
	{
		std::string name = c;
		return varToAddress[name];
	}
	else
	{
		if (env->KeyValueFound(c))
		{
			return env->readAddr(c);
		}
		else
		{
			return _nameToAddress(env->getFEnv(), c);
		}
	}
}

inline int NameToAddress(char* c)
{
	return _nameToAddress(processor.getCurEnv(),c);
}




assignment::assignment(char* _idf, char* _op, expression* _exp)
	: idf(_idf),op(_op),exp(_exp)
{}

void assignment::execute()
{
	if(exp == nullptr)
	{
		if (compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 3 << " " << 100 << " " << " " << " " << NameToAddress(idf);
			if (anotationModeOn) std::cout << "// 拷贝指令,语义：MEM[z] = MEM[x],此处用法为：函数调用赋值  ";
			std::cout << std::endl;
		}

		MEM[NameToAddress(idf)] = MEM[100];
	}
	else
	{
		exp->evaluate_compiler(0);

		if (compileModeOn)
		{
			if (lineCounterModeOn) std::cout << LC << ": "; LC++;
			std::cout << 3 << " " << 0 << " " << " " << " " << NameToAddress(idf);
			if (anotationModeOn) std::cout << "// 拷贝指令,语义：MEM[z] = MEM[x],此处用法为：表达式计算后对变量赋值  ";
			std::cout << std::endl;
		}

		MEM[NameToAddress(idf)] = MEM[0];
	}
	
	//return assignValueToIdf(idf,exp->evaluate());
}

output::output(char* _op, expression* exp_)
	:op(_op), exp(exp_)
{}

extern bool output_banned;
void output::execute()
{
	exp->evaluate_compiler(0);
	if(compileModeOn)
	{ // TODO-4 目前output只能处理 expression作为输出对象，可否尝试拓展至函数返回值？
		if (lineCounterModeOn) std::cout << LC << ": "; LC++;
		std::cout << 50 << " " << 0 << " " << " " << " " << " ";
		if (anotationModeOn) std::cout << "// output MEM[0] ";
		std::cout << std::endl;
	}
	if(!output_banned)
	{
		std::cout << MEM[0] << std::endl; 
	}
}


seqStatement::seqStatement(Lexer* lexer)
{
	
}


void seqStatement::execute()
{
	for(statement* p:seq)
	{
		p->execute();
	}
	
}
seqStatement::~seqStatement()
{
	for (statement* p : seq)
	{
		delete p;
	}
}



void varDecl::execute()
{
	std::string name = var;
	if(myEnv->isGlobalEnv())
	{
		varToAddress[name] = 101 + global_var_cnt;
		global_var_cnt++;
	}
}
